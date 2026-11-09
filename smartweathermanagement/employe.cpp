#include "employe.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QDateTime>
#include <QPageLayout>
#include <QFile>

Employe::Employe() {
    id_employe = "";
    nom = "";
    prenom = "";
    email = "";
    poste = "";
    structure = "";
    gouvernorat = "";
    statut = "";
    type_horaire = "";
}

Employe::Employe(QString id, QString nom, QString prenom, QString email, QString poste,
                 QString structure, QString gouvernorat, QDate dateRecrutement, QString statut, QString horaire) {
    this->id_employe = id;
    this->nom = nom;
    this->prenom = prenom;
    this->email = email;
    this->poste = poste;
    this->structure = structure;
    this->gouvernorat = gouvernorat;
    this->date_recrutement = dateRecrutement;
    this->statut = statut;
    this->type_horaire = horaire;
}

bool Employe::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO Employe (ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire) "
                  "VALUES (:id, :nom, :prenom, :email, :poste, :structure, :gouvernorat, :date_recrutement, :statut, :type_horaire)");

    query.bindValue(":id", id_employe);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":structure", structure);
    query.bindValue(":gouvernorat", gouvernorat);
    query.bindValue(":date_recrutement", date_recrutement.toString("yyyy-MM-dd"));
    query.bindValue(":statut", statut);
    query.bindValue(":type_horaire", type_horaire);

    if(query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur d'ajout Employe :" << query.lastError().text();
        return false;
    }
}

QSqlQueryModel* Employe::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire FROM Employe");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Poste"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Structure"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Gouvernorat"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Recrutement"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("Horaire"));

    return model;
}

bool Employe::supprimer(QString id) {
    QSqlQuery query;
    query.prepare("DELETE FROM Employe WHERE ID_Employe = :id");
    query.bindValue(":id", id);

    if(query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur de suppression Employe :" << query.lastError().text();
        return false;
    }
}

bool Employe::modifier() {
    QSqlQuery query;
    query.prepare("UPDATE Employe SET Nom = :nom, Prenom = :prenom, Email = :email, Poste = :poste, "
                  "Structure = :structure, Gouvernorat = :gouvernorat, Date_Recrutement = :date_recrutement, "
                  "Statut = :statut, Type_Horaire = :type_horaire WHERE ID_Employe = :id");

    query.bindValue(":id", id_employe);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":structure", structure);
    query.bindValue(":gouvernorat", gouvernorat);
    query.bindValue(":date_recrutement", date_recrutement.toString("yyyy-MM-dd"));
    query.bindValue(":statut", statut);
    query.bindValue(":type_horaire", type_horaire);

    if(query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur de modification Employe :" << query.lastError().text();
        return false;
    }
}

bool Employe::archiver(QString id) {
    QSqlQuery query;
    query.prepare("UPDATE Employe SET Statut = 'Archivé' WHERE ID_Employe = :id");
    query.bindValue(":id", id);
    if(query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur d'archivage Employe :" << query.lastError().text();
        return false;
    }
}

bool Employe::exporterListePDF(const QString &cheminFichier, const QString &editeur) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageOrientation(QPageLayout::Landscape);

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'/>";
    html += "<style>";
    html += "body { font-family: 'Arial', sans-serif; margin: 15px; color: #37474F; }";
    html += ".header { text-align: center; border-bottom: 3px solid #1E88E5; padding-bottom: 10px; margin-bottom: 15px; }";
    html += ".gov { font-size: 11px; text-transform: uppercase; color: #78909C; letter-spacing: 1px; margin-bottom: 3px; }";
    html += ".title { font-size: 22px; font-weight: bold; color: #1E88E5; margin: 0; }";
    html += ".subtitle { font-size: 13px; color: #37474F; margin-top: 4px; font-weight: bold; }";
    html += ".meta-box { background: #F5F7FA; border: 1px solid #D5DEE8; border-radius: 6px; padding: 10px 14px; margin-bottom: 15px; font-size: 11px; }";
    html += "table { width: 100%; border-collapse: collapse; margin-top: 10px; font-size: 10.5px; }";
    html += "th { background: #1E88E5; color: white; padding: 8px 6px; text-align: left; font-size: 11px; }";
    html += "td { padding: 7px 6px; border-bottom: 1px solid #E3EAF2; }";
    html += "tr:nth-child(even) { background-color: #F8FAFC; }";
    html += ".badge-actif { background-color: #2E7D32; color: white; padding: 3px 8px; border-radius: 4px; font-weight: bold; font-size: 9.5px; }";
    html += ".badge-conge { background-color: #F57C00; color: white; padding: 3px 8px; border-radius: 4px; font-weight: bold; font-size: 9.5px; }";
    html += ".badge-stagiaire { background-color: #0288D1; color: white; padding: 3px 8px; border-radius: 4px; font-weight: bold; font-size: 9.5px; }";
    html += ".badge-archive { background-color: #78909C; color: white; padding: 3px 8px; border-radius: 4px; font-weight: bold; font-size: 9.5px; }";
    html += ".badge-garde { background-color: #7B1FA2; color: white; padding: 2px 6px; border-radius: 3px; font-size: 9.5px; }";
    html += ".footer { margin-top: 25px; font-size: 10px; color: #78909C; border-top: 1px solid #D5DEE8; padding-top: 8px; }";
    html += "</style></head><body>";

    html += "<div class='header'>";
    html += "<div class='gov'>République Tunisienne • Ministère du Transport</div>";
    html += "<div class='title'>INSTITUT NATIONAL DE LA MÉTÉOROLOGIE</div>";
    html += "<div class='subtitle'>Direction des Ressources Humaines • Registre Officiel du Personnel (Module 1)</div>";
    html += "</div>";

    QSqlQuery qCount("SELECT COUNT(*) FROM employe");
    int total = (qCount.next()) ? qCount.value(0).toInt() : 0;

    html += "<div class='meta-box'>";
    html += "<b>Date d'édition :</b> " + QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm:ss") + " &nbsp;|&nbsp; ";
    html += "<b>Édité par :</b> " + editeur + " &nbsp;|&nbsp; ";
    html += "<b>Total des employés répertoriés :</b> " + QString::number(total) + " agents &nbsp;|&nbsp; ";
    html += "<b>Usage :</b> Registre officiel conforme aux normes INM";
    html += "</div>";

    html += "<table>";
    html += "<thead><tr>";
    html += "<th>ID</th>";
    html += "<th>Nom & Prénom</th>";
    html += "<th>Email</th>";
    html += "<th>Poste</th>";
    html += "<th>Structure</th>";
    html += "<th>Gouvernorat</th>";
    html += "<th>Recrutement</th>";
    html += "<th>Statut</th>";
    html += "<th>Régime Horaire</th>";
    html += "</tr></thead><tbody>";

    QSqlQuery q("SELECT id_employe, nom, prenom, email, poste, structure, gouvernorat, "
                "date_recrutement, statut, type_horaire FROM employe ORDER BY id_employe ASC");

    while (q.next()) {
        QString statut = q.value(8).toString();
        QString badgeClass = "badge-actif";
        if (statut == "En congé") badgeClass = "badge-conge";
        else if (statut == "Stagiaire") badgeClass = "badge-stagiaire";
        else if (statut == "Archivé") badgeClass = "badge-archive";

        QString horaire = q.value(9).toString();
        QString horaireHtml = (horaire == "Garde 24h")
            ? "<span class='badge-garde'>Garde 24h</span>"
            : "<span>Jour</span>";

        html += "<tr>";
        html += QString("<td><b>#%1</b></td>").arg(q.value(0).toString());
        html += QString("<td><b>%1 %2</b></td>").arg(q.value(1).toString(), q.value(2).toString());
        html += QString("<td>%1</td>").arg(q.value(3).toString());
        html += QString("<td>%1</td>").arg(q.value(4).toString());
        html += QString("<td>%1</td>").arg(q.value(5).toString());
        html += QString("<td>%1</td>").arg(q.value(6).toString());
        html += QString("<td>%1</td>").arg(q.value(7).toString());
        html += QString("<td><span class='%1'>%2</span></td>").arg(badgeClass, statut);
        html += QString("<td>%1</td>").arg(horaireHtml);
        html += "</tr>";
    }

    html += "</tbody></table>";

    html += "<div class='footer'>";
    html += "<table width='100%' style='border:none;'><tr>";
    html += "<td style='border:none;'>SMART WEATHER MANAGEMENT • Syst&egrave;me d'Information INM</td>";
    html += "<td style='border:none; text-align:right;'>Document certifi&eacute; conforme • Page 1</td>";
    html += "</tr></table>";
    html += "</div>";

    html += "</body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

bool Employe::exporterFichePDF(const QString &id, const QString &cheminFichier, const QString &editeur) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageOrientation(QPageLayout::Portrait);

    QSqlQuery q;
    q.prepare("SELECT id_employe, nom, prenom, email, poste, structure, gouvernorat, "
              "date_recrutement, statut, type_horaire FROM employe WHERE id_employe = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return false;

    QString nom = q.value(1).toString();
    QString prenom = q.value(2).toString();
    QString email = q.value(3).toString();
    QString poste = q.value(4).toString();
    QString structure = q.value(5).toString();
    QString gouv = q.value(6).toString();
    QString dateRecrut = q.value(7).toString();
    QString statut = q.value(8).toString();
    QString horaire = q.value(9).toString();

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'/>";
    html += "<style>";
    html += "body { font-family: 'Arial', sans-serif; margin: 25px; color: #37474F; }";
    html += ".header { text-align: center; border-bottom: 3px solid #1E88E5; padding-bottom: 12px; margin-bottom: 20px; }";
    html += ".gov { font-size: 11px; text-transform: uppercase; color: #78909C; letter-spacing: 1px; }";
    html += ".title { font-size: 24px; font-weight: bold; color: #1E88E5; margin: 4px 0; }";
    html += ".subtitle { font-size: 14px; color: #37474F; font-weight: bold; }";
    html += ".card { background: #FFFFFF; border: 1px solid #D5DEE8; border-radius: 8px; padding: 16px; margin-bottom: 16px; }";
    html += ".card-title { font-size: 14px; font-weight: bold; color: #1E88E5; border-bottom: 1px solid #E3EAF2; padding-bottom: 6px; margin-bottom: 12px; }";
    html += ".row { margin-bottom: 8px; font-size: 12px; }";
    html += ".label { font-weight: bold; color: #546E7A; width: 180px; display: inline-block; }";
    html += ".value { color: #263238; font-weight: 500; }";
    html += ".badge { display: inline-block; padding: 4px 10px; border-radius: 4px; font-size: 11px; font-weight: bold; color: white; background: #1E88E5; }";
    html += ".signature-box { margin-top: 35px; border-top: 1px dashed #B0BEC5; padding-top: 15px; }";
    html += "</style></head><body>";

    html += "<div class='header'>";
    html += "<div class='gov'>R&eacute;publique Tunisienne &bull; Minist&egrave;re du Transport</div>";
    html += "<div class='title'>INSTITUT NATIONAL DE LA M&Eacute;T&Eacute;OROLOGIE</div>";
    html += "<div class='subtitle'>DOSSIER DU PERSONNEL &bull; FICHE INDIVIDUELLE DE L'EMPLOY&Eacute;</div>";
    html += "</div>";

    html += "<div class='card'>";
    html += "<div class='card-title'>IDENTIT&Eacute; ET COORDONN&Eacute;ES</div>";
    html += QString("<div class='row'><span class='label'>Identifiant Agent :</span><span class='value'>#%1</span></div>").arg(id);
    html += QString("<div class='row'><span class='label'>Nom et Pr&eacute;nom :</span><span class='value'><b>%1 %2</b></span></div>").arg(nom, prenom);
    html += QString("<div class='row'><span class='label'>Adresse Email :</span><span class='value'>%1</span></div>").arg(email);
    html += QString("<div class='row'><span class='label'>Statut Administratif :</span><span class='badge'>%1</span></div>").arg(statut);
    html += "</div>";

    html += "<div class='card'>";
    html += "<div class='card-title'>AFFECTATION PROFESSIONNELLE</div>";
    html += QString("<div class='row'><span class='label'>Poste Op&eacute;rationnel :</span><span class='value'><b>%1</b></span></div>").arg(poste);
    html += QString("<div class='row'><span class='label'>Structure de Rattachement :</span><span class='value'>%1</span></div>").arg(structure);
    html += QString("<div class='row'><span class='label'>Gouvernorat d'Affectation :</span><span class='value'>%1</span></div>").arg(gouv);
    html += "</div>";

    html += "<div class='card'>";
    html += "<div class='card-title'>MODALIT&Eacute;S DE SERVICE & HORAIRES</div>";
    html += QString("<div class='row'><span class='label'>Date de Recrutement :</span><span class='value'>%1</span></div>").arg(dateRecrut);
    html += QString("<div class='row'><span class='label'>R&eacute;gime de Travail :</span><span class='value'><b>%1</b></span></div>").arg(horaire);
    html += "</div>";

    html += "<div class='signature-box'>";
    html += "<table width='100%' style='border:none;'>";
    html += "<tr>";
    html += QString("<td style='width:50%; font-size:11px; vertical-align:top;'><b>Date d'&eacute;dition :</b> %1<br><b>Visa RH par :</b> %2</td>")
                .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm"), editeur);
    html += "<td style='width:50%; text-align:right; font-size:11px; vertical-align:top;'><b>Cachet & Signature Direction RH :</b><br><br><br>___________________________________</td>";
    html += "</tr>";
    html += "</table>";
    html += "</div>";

    html += "</body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

bool Employe::existe(const QString &id) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Employe WHERE ID_Employe = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QSqlQueryModel* Employe::trier(const QString &critere, const QString &ordre) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QString col = "ID_Employe";
    if (critere == "Nom") col = "Nom";
    else if (critere == "Poste") col = "Poste";
    else if (critere == "Date de recrutement") col = "Date_Recrutement";
    else if (critere == "Structure") col = "Structure";
    else if (critere == "Gouvernorat") col = "Gouvernorat";
    else if (critere == "Statut") col = "Statut";
    else if (critere == "ID") col = "ID_Employe";

    QString direction = (ordre.toUpper() == "DESC") ? "DESC" : "ASC";
    QString sql = QString("SELECT ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire "
                          "FROM Employe ORDER BY %1 %2").arg(col, direction);

    model->setQuery(sql);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Poste"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Structure"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Gouvernorat"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Recrutement"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("Horaire"));
    return model;
}

QSqlQueryModel* Employe::rechercher(const QString &texte, const QString &critere, const QString &tri) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QString colTri = "Nom";
    if (tri == "Poste") colTri = "Poste";
    else if (tri == "Date de recrutement") colTri = "Date_Recrutement";
    else if (tri == "Structure") colTri = "Structure";
    else if (tri == "Gouvernorat") colTri = "Gouvernorat";
    else if (tri == "ID") colTri = "ID_Employe";

    QSqlQuery query;
    if (critere == "Tous" || critere.isEmpty()) {
        query.prepare(QString("SELECT ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire "
                              "FROM Employe WHERE (UPPER(Nom) LIKE UPPER(:val) OR UPPER(Prenom) LIKE UPPER(:val) "
                              "OR UPPER(Email) LIKE UPPER(:val) OR UPPER(Poste) LIKE UPPER(:val) "
                              "OR UPPER(Gouvernorat) LIKE UPPER(:val) OR UPPER(ID_Employe) LIKE UPPER(:val)) "
                              "ORDER BY %1 ASC").arg(colTri));
    } else {
        QString colRecherche = "Nom";
        if (critere == "Poste") colRecherche = "Poste";
        else if (critere == "Structure") colRecherche = "Structure";
        else if (critere == "Gouvernorat") colRecherche = "Gouvernorat";
        else if (critere == "ID") colRecherche = "ID_Employe";
        else if (critere == "Email") colRecherche = "Email";

        query.prepare(QString("SELECT ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire "
                              "FROM Employe WHERE UPPER(%1) LIKE UPPER(:val) ORDER BY %2 ASC").arg(colRecherche, colTri));
    }
    query.bindValue(":val", "%" + texte + "%");
    query.exec();

    model->setQuery(std::move(query));
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Poste"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Structure"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Gouvernorat"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Recrutement"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("Horaire"));
    return model;
}