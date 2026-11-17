#include "equipement.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDateTime>
#include <QFile>
#include <QPdfWriter>
#include <QPageSize>
#include <QPageLayout>
#include <QTextDocument>

static QString g_erreur;

// Meme colonnes, meme ordre, partout. La derniere (ID_STATION) est masquee dans le tableau.
static const QString SELECT_BASE =
    "SELECT e.ID_EQUIPEMENT, e.NOM_EQUIPEMENT, e.TYPE_EQUIPEMENT, e.MARQUE, e.NUMERO_SERIE, "
    "TO_CHAR(e.DATE_INSTALLATION, 'DD/MM/YYYY'), e.STATUT, s.NOM_STATION, e.ID_STATION "
    "FROM EQUIPEMENT e JOIN STATION s ON s.ID_STATION = e.ID_STATION";

Equipement::Equipement()
    : id_equipement(0), date_installation(QDate::currentDate()),
      statut("Opérationnel") {}

Equipement::Equipement(int id, const QString &nom, const QString &type, const QString &marque,
                       const QString &serie, const QDate &date, const QString &statut, const QString &idStation)
    : id_equipement(id), nom_equipement(nom), type_equipement(type), marque(marque),
      numero_serie(serie), date_installation(date), statut(statut), id_station(idStation) {}

QString Equipement::verifier() const
{
    if (nom_equipement.trimmed().isEmpty())
        return "Le nom de l'équipement est obligatoire.";
    if (nom_equipement.size() > 80)
        return "Le nom est trop long (80 caractères maximum).";
    if (marque.size() > 40)
        return "La marque est trop longue (40 caractères maximum).";
    // Numero de serie : lettres, chiffres et tirets, 3 a 40 caracteres
    static const QRegularExpression reSerie("^[A-Za-z0-9-]{3,40}$");
    if (!reSerie.match(numero_serie).hasMatch())
        return "Le numéro de série doit contenir 3 à 40 lettres, chiffres ou tirets.";
    if (!date_installation.isValid() || date_installation > QDate::currentDate())
        return "La date d'installation est invalide (elle ne peut pas être dans le futur).";
    if (id_station.isEmpty())
        return "Choisissez une station.";
    return "";
}

bool Equipement::ajouter()
{
    QSqlQuery q;
    q.prepare("INSERT INTO EQUIPEMENT (ID_EQUIPEMENT, NOM_EQUIPEMENT, TYPE_EQUIPEMENT, MARQUE, "
              "NUMERO_SERIE, DATE_INSTALLATION, STATUT, ID_STATION) "
              "VALUES (SEQ_EQUIPEMENT.NEXTVAL, :nom, :type, :marque, :serie, :date, :statut, :station)");
    q.bindValue(":nom", nom_equipement.trimmed());
    q.bindValue(":type", type_equipement);
    q.bindValue(":marque", marque.trimmed());
    q.bindValue(":serie", numero_serie.trimmed());
    q.bindValue(":date", date_installation);
    q.bindValue(":statut", statut);
    q.bindValue(":station", id_station);
    if (!q.exec()) { g_erreur = q.lastError().text(); return false; }
    return true;
}

QSqlQueryModel *Equipement::afficher(const QString &recherche, const QString &tri)
{
    // Tri : liste blanche (jamais de texte libre dans ORDER BY)
    static const QMap<QString, QString> colonnes = {
        {"Nom", "e.NOM_EQUIPEMENT"}, {"Type", "e.TYPE_EQUIPEMENT"},
        {"Statut", "e.STATUT"}, {"Date d'installation", "e.DATE_INSTALLATION"},
        {"Station", "s.NOM_STATION"}
    };

    QString sql = SELECT_BASE;
    const bool filtre = !recherche.trimmed().isEmpty();
    if (filtre)
        sql += " WHERE UPPER(e.NOM_EQUIPEMENT) LIKE UPPER(:v1) OR UPPER(e.TYPE_EQUIPEMENT) LIKE UPPER(:v2) "
               "OR UPPER(e.MARQUE) LIKE UPPER(:v3) OR UPPER(e.NUMERO_SERIE) LIKE UPPER(:v4) "
               "OR UPPER(s.NOM_STATION) LIKE UPPER(:v5)";
    sql += colonnes.contains(tri) ? " ORDER BY " + colonnes.value(tri) : " ORDER BY e.ID_EQUIPEMENT";

    QSqlQuery q;
    q.prepare(sql);
    if (filtre) {
        const QString v = "%" + recherche.trimmed() + "%";
        for (const char *p : {":v1", ":v2", ":v3", ":v4", ":v5"})
            q.bindValue(p, v);
    }
    q.exec();

    auto *model = new QSqlQueryModel;
    model->setQuery(std::move(q));
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Marque");
    model->setHeaderData(4, Qt::Horizontal, "N° série");
    model->setHeaderData(5, Qt::Horizontal, "Installation");
    model->setHeaderData(6, Qt::Horizontal, "Statut");
    model->setHeaderData(7, Qt::Horizontal, "Station");
    return model;
}

bool Equipement::modifier()
{
    QSqlQuery q;
    q.prepare("UPDATE EQUIPEMENT SET NOM_EQUIPEMENT = :nom, TYPE_EQUIPEMENT = :type, MARQUE = :marque, "
              "NUMERO_SERIE = :serie, DATE_INSTALLATION = :date, STATUT = :statut, ID_STATION = :station "
              "WHERE ID_EQUIPEMENT = :id");
    q.bindValue(":nom", nom_equipement.trimmed());
    q.bindValue(":type", type_equipement);
    q.bindValue(":marque", marque.trimmed());
    q.bindValue(":serie", numero_serie.trimmed());
    q.bindValue(":date", date_installation);
    q.bindValue(":statut", statut);
    q.bindValue(":station", id_station);
    q.bindValue(":id", id_equipement);
    if (!q.exec()) { g_erreur = q.lastError().text(); return false; }
    return q.numRowsAffected() > 0;
}

bool Equipement::supprimer(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM EQUIPEMENT WHERE ID_EQUIPEMENT = :id");
    q.bindValue(":id", id);
    if (!q.exec()) { g_erreur = q.lastError().text(); return false; }
    return q.numRowsAffected() > 0;
}

QMap<QString, int> Equipement::repartition(const QString &champ)
{
    // Liste blanche : le nom de colonne n'est jamais pris tel quel depuis l'exterieur
    const QString col = (champ == "Type") ? "TYPE_EQUIPEMENT" : "STATUT";
    QMap<QString, int> resultat;
    QSqlQuery q("SELECT " + col + ", COUNT(*) FROM EQUIPEMENT GROUP BY " + col);
    while (q.next())
        resultat[q.value(0).toString()] = q.value(1).toInt();
    return resultat;
}

bool Equipement::exporterPdf(const QString &fichier, const QString &recherche, const QString &tri)
{
    QScopedPointer<QSqlQueryModel> m(afficher(recherche, tri));
    while (m->canFetchMore()) m->fetchMore();     // Oracle charge les lignes par paquets

    QString html = "<h2 align='center'>Liste des équipements</h2>"
                   "<p align='center'>Généré le " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm") +
                   " — " + QString::number(m->rowCount()) + " équipement(s)</p>"
                   "<table border='1' cellspacing='0' cellpadding='4' width='100%'><tr bgcolor='#1F3B57'>";
    for (int c = 0; c < 8; ++c)                   // la colonne 8 (ID_STATION) n'est pas affichee
        html += "<th><font color='white'>" + m->headerData(c, Qt::Horizontal).toString().toHtmlEscaped() + "</font></th>";
    html += "</tr>";
    for (int r = 0; r < m->rowCount(); ++r) {
        html += (r % 2) ? "<tr bgcolor='#F4F8FC'>" : "<tr>";
        for (int c = 0; c < 8; ++c)
            html += "<td>" + m->data(m->index(r, c)).toString().toHtmlEscaped() + "</td>";
        html += "</tr>";
    }
    html += "</table>";

    QPdfWriter pdf(fichier);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setPageOrientation(QPageLayout::Landscape);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&pdf);
    return QFile::exists(fichier);
}

QString Equipement::derniereErreur() { return g_erreur; }
