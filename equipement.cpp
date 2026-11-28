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

#include <QSqlDatabase>
#include <algorithm>

static QString g_erreur;

// Ecrit un document HTML en PDF A4 paysage (partage par les deux exports)
static bool ecrirePdf(const QString &html, const QString &fichier)
{
    QPdfWriter pdf(fichier);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setPageOrientation(QPageLayout::Landscape);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&pdf);
    return QFile::exists(fichier);
}

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
    return ecrirePdf(html, fichier);
}

// ---------------------------------------------------------------------------
// Metier innovant 1 : score de sante
// ---------------------------------------------------------------------------
int Equipement::calculerScore(const QString &type, const QDate &installation, const QString &statut,
                              const QString &statutStation, double *ageAns)
{
    // Duree de vie moyenne (en annees) par type d'equipement
    static const QMap<QString, int> vie = {
        {"Capteur", 6}, {"Anémomètre", 5}, {"Baromètre", 8},
        {"Pluviomètre", 7}, {"Thermomètre", 6}, {"Autre", 5}
    };
    const double age = installation.isValid() ? installation.daysTo(QDate::currentDate()) / 365.25 : 0.0;
    if (ageAns) *ageAns = age;

    const double usure = qBound(0.0, age / vie.value(type, 5), 1.0);   // 0 = neuf, 1 = fin de vie
    int score = 100 - qRound(60 * usure);                              // l'age pese jusqu'a 60 points
    if (statut == "En panne")             score -= 40;
    else if (statut == "En maintenance")  score -= 20;
    if (statutStation == "En maintenance") score -= 10;                // la station est deja a l'arret
    return qBound(0, score, 100);
}

QString Equipement::niveauSante(int score)
{
    return score < 40 ? "Critique" : (score < 70 ? "À surveiller" : "Bon");
}

QList<SanteEquipement> Equipement::classementSante()
{
    QList<SanteEquipement> liste;
    QSqlQuery q("SELECT e.ID_EQUIPEMENT, e.NOM_EQUIPEMENT, e.TYPE_EQUIPEMENT, s.NOM_STATION, "
                "e.DATE_INSTALLATION, e.STATUT, s.STATUT "
                "FROM EQUIPEMENT e JOIN STATION s ON s.ID_STATION = e.ID_STATION");
    while (q.next()) {
        SanteEquipement h;
        h.id = q.value(0).toInt();
        h.nom = q.value(1).toString();
        h.type = q.value(2).toString();
        h.station = q.value(3).toString();
        h.statut = q.value(5).toString();
        h.score = calculerScore(h.type, q.value(4).toDate(), h.statut, q.value(6).toString(), &h.ageAns);
        h.niveau = niveauSante(h.score);
        liste.append(h);
    }
    std::sort(liste.begin(), liste.end(), [](const SanteEquipement &a, const SanteEquipement &b) {
        return a.score != b.score ? a.score < b.score : a.nom < b.nom;
    });
    return liste;
}

bool Equipement::exporterSantePdf(const QString &fichier)
{
    const auto liste = classementSante();
    QString html = "<h2 align='center'>Équipements à remplacer – score de santé</h2>"
                   "<p align='center'>Généré le " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm") +
                   " — " + QString::number(liste.size()) + " équipement(s)</p>"
                   "<table border='1' cellspacing='0' cellpadding='4' width='100%'><tr bgcolor='#1F3B57'>";
    for (const char *t : {"Équipement", "Type", "Station", "Statut", "Âge (ans)", "Score", "Niveau"})
        html += QString("<th><font color='white'>%1</font></th>").arg(QString::fromUtf8(t));
    html += "</tr>";
    for (const auto &h : liste) {
        const QString fond = h.niveau == "Critique" ? "#FDECEA" : (h.niveau == "Bon" ? "#E8F5E9" : "#FFF4E0");
        html += "<tr bgcolor='" + fond + "'><td>" + h.nom.toHtmlEscaped() + "</td><td>" + h.type.toHtmlEscaped() +
                "</td><td>" + h.station.toHtmlEscaped() + "</td><td>" + h.statut.toHtmlEscaped() +
                "</td><td>" + QString::number(h.ageAns, 'f', 1) + "</td><td>" + QString::number(h.score) +
                "</td><td>" + h.niveau.toHtmlEscaped() + "</td></tr>";
    }
    html += "</table>";
    return ecrirePdf(html, fichier);
}

// ---------------------------------------------------------------------------
// Metier innovant 2 : panne -> intervention de maintenance
// ---------------------------------------------------------------------------
bool Equipement::creerInterventionPanne(int idEquipement, QString &message)
{
    QSqlQuery e;
    e.prepare("SELECT NOM_EQUIPEMENT, NUMERO_SERIE, STATUT, ID_STATION FROM EQUIPEMENT WHERE ID_EQUIPEMENT = :id");
    e.bindValue(":id", idEquipement);
    if (!e.exec() || !e.next()) { message = "Équipement introuvable."; return false; }
    const QString nom = e.value(0).toString(), serie = e.value(1).toString();
    const QVariant station = e.value(3);
    if (e.value(2).toString() != "En panne") {
        message = "Seul un équipement « En panne » peut déclencher une intervention.\n"
                  "Passez d'abord son statut à « En panne » (bouton Modifier).";
        return false;
    }

    // Le marqueur [EQ#id] dans le rapport relie l'intervention a l'equipement (et evite les doublons)
    const QString marqueur = QString("[EQ#%1]").arg(idEquipement);
    QSqlQuery d;
    d.prepare("SELECT COUNT(*) FROM INTERVENTION WHERE STATUT IN ('Planifiée','En cours') AND ARCHIVE = 0 "
              "AND DBMS_LOB.INSTR(RAPPORT_INTERVENTION, :m) > 0");
    d.bindValue(":m", marqueur);
    if (d.exec() && d.next() && d.value(0).toInt() > 0) {
        message = "Une intervention est déjà ouverte pour cet équipement.";
        return false;
    }

    // Technicien : poste "technicien" en priorite, puis le moins charge (interventions ouvertes)
    QSqlQuery t("SELECT e.ID_EMPLOYE FROM EMPLOYE e WHERE e.STATUT = 'Actif' ORDER BY "
                "CASE WHEN UPPER(e.POSTE) LIKE '%TECH%' THEN 0 ELSE 1 END, "
                "(SELECT COUNT(*) FROM INTERVENTION i WHERE i.ID_EMPLOYE = e.ID_EMPLOYE "
                "AND i.STATUT IN ('Planifiée','En cours')), e.ID_EMPLOYE");
    if (!t.next()) { message = "Aucun employé actif pour assurer l'intervention."; return false; }
    const QVariant technicien = t.value(0);

    // ID : meme methode que le module Maintenance (MAX + 1)
    // ponytail: pas de verrou, deux creations simultanees pourraient entrer en conflit.
    QSqlQuery n("SELECT COALESCE(MAX(ID_INTERVENTION), 0) + 1 FROM INTERVENTION");
    n.next();
    const int idIntervention = n.value(0).toInt();

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();
    QSqlQuery i;
    i.prepare("INSERT INTO INTERVENTION (ID_INTERVENTION, ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, PRIORITE, "
              "DATE_CREATION, DATE_PLANIFIEE, STATUT, RAPPORT_INTERVENTION) "
              "VALUES (:id, :st, :emp, 'Corrective', 'Haute', :cree, :plan, 'Planifiée', :rap)");
    i.bindValue(":id", idIntervention);
    i.bindValue(":st", station);
    i.bindValue(":emp", technicien);
    i.bindValue(":cree", QDateTime::currentDateTime());
    i.bindValue(":plan", QDateTime::currentDateTime().addDays(1));
    i.bindValue(":rap", QString("Panne de l'équipement « %1 » (n° série %2). %3").arg(nom, serie, marqueur));
    QSqlQuery u;
    u.prepare("UPDATE EQUIPEMENT SET STATUT = 'En maintenance' WHERE ID_EQUIPEMENT = :id");
    u.bindValue(":id", idEquipement);
    if (!i.exec() || !u.exec()) {
        g_erreur = (i.lastError().isValid() ? i.lastError() : u.lastError()).text();
        db.rollback();
        message = "Création impossible : " + g_erreur;
        return false;
    }
    db.commit();
    message = QString("Intervention n°%1 créée (priorité Haute, planifiée demain).\n"
                      "L'équipement passe « En maintenance ».").arg(idIntervention);
    return true;
}

QString Equipement::derniereErreur() { return g_erreur; }
