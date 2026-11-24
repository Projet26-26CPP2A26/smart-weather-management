#include "employe.h"
#include "qrcodegen.hpp"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QPrinter>
#include <QPainter>
#include <QPainterPath>
#include <QTextDocument>
#include <QDateTime>
#include <QPageLayout>
#include <QFile>
#include <QBuffer>
#include <QCryptographicHash>

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
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageSize(QPageSize(QPageSize::A4));
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
    doc.setPageSize(printer.pageLayout().paintRectPixels(printer.resolution()).size());
    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

bool Employe::exporterFichePDF(const QString &id, const QString &cheminFichier, const QString &editeur) {
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageSize(QPageSize(QPageSize::A4));
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

    // Génération du QR Code de vérification
    QString qrData = QString("INM-AGENT|ID:%1|NOM:%2|PRENOM:%3|POSTE:%4|STATUT:%5")
                         .arg(id, nom, prenom, poste, statut);
    QImage qrImg = genererQrCode(qrData, 140);
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    qrImg.save(&buffer, "PNG");
    QString qrBase64 = QString::fromLatin1(ba.toBase64());

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

    html += "<table width='100%' style='border:none; margin-bottom:15px;'><tr>";
    html += "<td style='border:none; vertical-align:top;'>";
    html += "<div class='card'>";
    html += "<div class='card-title'>IDENTIT&Eacute; ET COORDONN&Eacute;ES</div>";
    html += QString("<div class='row'><span class='label'>Identifiant Agent :</span><span class='value'><b>#%1</b></span></div>").arg(id);
    html += QString("<div class='row'><span class='label'>Nom et Pr&eacute;nom :</span><span class='value'><b>%1 %2</b></span></div>").arg(nom, prenom);
    html += QString("<div class='row'><span class='label'>Adresse Email :</span><span class='value'>%1</span></div>").arg(email);
    html += QString("<div class='row'><span class='label'>Statut Administratif :</span><span class='badge'>%1</span></div>").arg(statut);
    html += "</div>";
    html += "</td>";
    html += "<td style='border:none; width:150px; text-align:center; vertical-align:middle;'>";
    html += QString("<img src='data:image/png;base64,%1' width='120' height='120'/><br><span style='font-size:9px; color:#78909C;'>Contrôle Sécurisé QR</span>").arg(qrBase64);
    html += "</td></tr></table>";

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
    doc.setPageSize(printer.pageLayout().paintRectPixels(printer.resolution()).size());
    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

bool Employe::exporterAttestationPDF(const QString &id, const QString &cheminFichier, const QString &editeur) {
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);

    QSqlQuery q;
    q.prepare("SELECT id_employe, nom, prenom, email, poste, structure, gouvernorat, "
              "date_recrutement, statut, type_horaire FROM employe WHERE id_employe = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return false;

    QString nom = q.value(1).toString();
    QString prenom = q.value(2).toString();
    QString poste = q.value(4).toString();
    QString structure = q.value(5).toString();
    QString gouv = q.value(6).toString();
    QString dateRecrut = q.value(7).toString();
    QString statut = q.value(8).toString();
    QString horaire = q.value(9).toString();

    // Numéro de référence officiel
    QString refAttestation = QString("REF-INM-RH-%1-%2").arg(QDate::currentDate().toString("yyyy"), id);

    // QR Code d'authentification numérique
    QString hashSec = QString::fromLatin1(QCryptographicHash::hash((id + nom + prenom + "INM_SECRET_KEY").toUtf8(), QCryptographicHash::Sha256).toHex()).left(12).toUpper();
    QString qrData = QString("INM-ATTESTATION|REF:%1|ID:%2|AGENT:%3 %4|POSTE:%5|DATE:%6|SEC:%7")
                         .arg(refAttestation, id, nom, prenom, poste, QDate::currentDate().toString("yyyy-MM-dd"), hashSec);
    QImage qrImg = genererQrCode(qrData, 160);
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    qrImg.save(&buffer, "PNG");
    QString qrBase64 = QString::fromLatin1(ba.toBase64());

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'/>";
    html += "<style>";
    html += "body { font-family: 'Times New Roman', serif; margin: 30px; color: #1A202C; font-size: 12pt; }";
    html += ".header { text-align: center; border-bottom: 2px solid #0D47A1; padding-bottom: 12px; margin-bottom: 25px; }";
    html += ".rep { font-size: 12pt; text-transform: uppercase; font-weight: bold; letter-spacing: 2px; }";
    html += ".min { font-size: 10.5pt; text-transform: uppercase; margin-top: 3px; color: #4A5568; }";
    html += ".inm { font-size: 15pt; font-weight: bold; color: #0D47A1; margin-top: 6px; letter-spacing: 1px; }";
    html += ".ref-box { font-family: Arial, sans-serif; font-size: 10pt; color: #4A5568; margin-top: 15px; margin-bottom: 25px; }";
    html += ".title { text-align: center; font-size: 20pt; font-weight: bold; color: #0D47A1; text-decoration: underline; margin: 25px 0; letter-spacing: 2px; }";
    html += ".content { font-size: 12.5pt; line-height: 1.8; text-align: justify; margin-bottom: 20px; text-indent: 30px; }";
    html += ".agent-box { background: #F8FAFC; border: 1.5px solid #CBD5E1; border-radius: 8px; padding: 16px 20px; margin: 18px 0; font-family: Arial, sans-serif; font-size: 11.5pt; line-height: 1.6; }";
    html += ".conclusion { font-size: 12.5pt; line-height: 1.8; text-align: justify; margin-top: 20px; text-indent: 30px; }";
    html += ".sign-table { width: 100%; margin-top: 40px; font-family: Arial, sans-serif; }";
    html += "</style></head><body>";

    html += "<div class='header'>";
    html += "<div class='rep'>R&eacute;publique Tunisienne</div>";
    html += "<div class='min'>Minist&egrave;re du Transport</div>";
    html += "<div class='inm'>INSTITUT NATIONAL DE LA M&Eacute;T&Eacute;OROLOGIE (INM)</div>";
    html += "<div style='font-size:12px; color:#64748B; margin-top:4px;'>Direction des Ressources Humaines &bull; Service Central du Personnel</div>";
    html += "</div>";

    html += "<div class='ref-box'>";
    html += QString("<b>R&eacute;f&eacute;rence Officielle :</b> %1 &nbsp;&nbsp;&bull;&nbsp;&nbsp; <b>Date :</b> %2")
                .arg(refAttestation, QDate::currentDate().toString("dd MMMM yyyy"));
    html += "</div>";

    html += "<div class='title'>ATTESTATION DE TRAVAIL</div>";

    html += "<div class='content'>";
    html += "Le Directeur des Ressources Humaines de l'Institut National de la M&eacute;t&eacute;orologie (INM), soussign&eacute;, certifie par la pr&eacute;sente que :";
    html += "</div>";

    html += "<div class='agent-box'>";
    html += QString("<p style='margin:4px 0;'><b>Monsieur / Madame :</b> <span style='font-size:15px; color:#0D47A1;'>%1 %2</span></p>").arg(nom.toUpper(), prenom);
    html += QString("<p style='margin:4px 0;'><b>Identifiant National Agent :</b> <b>#%1</b></p>").arg(id);
    html += QString("<p style='margin:4px 0;'><b>Fonction / Poste :</b> %1</p>").arg(poste);
    html += QString("<p style='margin:4px 0;'><b>Structure de Rattachement :</b> %1 (Gouvernorat de %2)</p>").arg(structure, gouv);
    html += QString("<p style='margin:4px 0;'><b>Date de Prise de Fonction :</b> %1</p>").arg(dateRecrut);
    html += QString("<p style='margin:4px 0;'><b>Statut Op&eacute;rationnel Actuel :</b> %1 (R&eacute;gime : %2)</p>").arg(statut, horaire);
    html += "</div>";

    html += "<div class='content'>";
    html += "Est r&eacute;guli&egrave;rement employ&eacute;(e) au sein de nos services en qualit&eacute; d'agent titulaire permanent. L'int&eacute;ress&eacute;(e) exerce ses fonctions avec professionnalisme et d&eacute;vouement, et se trouve &agrave; ce jour libre de tout engagement p&eacute;cuniaire ou administratif &agrave; l'&eacute;gard de notre &eacute;tablissement.";
    html += "</div>";

    html += "<div class='conclusion'>";
    html += "La pr&eacute;sente attestation lui est d&eacute;livr&eacute;e &agrave; sa demande pour <b>servir et valoir ce que de droit</b>.";
    html += "</div>";

    html += "<table class='sign-table' style='border:none;'><tr>";
    html += "<td style='border:none; width:40%; text-align:center; vertical-align:middle;'>";
    html += QString("<img src='data:image/png;base64,%1' width='120' height='120'/><br>")
                .arg(qrBase64);
    html += QString("<span style='font-size:9.5px; color:#64748B;'>V&eacute;rification d'authenticit&eacute;<br>Signature &bull; %1</span>").arg(hashSec);
    html += "</td>";
    html += "<td style='border:none; width:60%; text-align:right; vertical-align:top; font-size:12px;'>";
    html += QString("Fait &agrave; Tunis, le %1<br>").arg(QDate::currentDate().toString("dd/MM/yyyy"));
    html += "<b>Pour le Ministre du Transport</b><br>";
    html += "<b>Par D&eacute;l&eacute;gation, Le Directeur des Ressources Humaines</b><br>";
    html += QString("<i>%1</i><br><br><br><br>").arg(editeur);
    html += "__________________________________________<br>";
    html += "<span style='font-size:10px; color:#64748B;'>[Cachet Officiel & Signature R&eacute;glementaire]</span>";
    html += "</td>";
    html += "</tr></table>";

    html += "</body></html>";

    QTextDocument doc;
    doc.setPageSize(printer.pageLayout().paintRectPixels(printer.resolution()).size());
    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

bool Employe::exporterStatsPDF(const QString &cheminFichier, const QString &editeur) {
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);

    QSqlQuery qTotal("SELECT COUNT(*) FROM Employe");
    int total = (qTotal.next()) ? qTotal.value(0).toInt() : 0;
    if (total == 0) total = 1;

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'/>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 25px; color: #1E293B; }";
    html += ".header { text-align: center; border-bottom: 3px solid #1E88E5; padding-bottom: 10px; margin-bottom: 20px; }";
    html += ".title { font-size: 22px; font-weight: bold; color: #1E88E5; }";
    html += ".sub { font-size: 13px; color: #64748B; margin-top: 4px; }";
    html += ".card { background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 8px; padding: 14px; margin-bottom: 18px; }";
    html += ".card-h { font-size: 14px; font-weight: bold; color: #0F172A; margin-bottom: 10px; border-bottom: 1px solid #CBD5E1; padding-bottom: 4px; }";
    html += "table { width: 100%; border-collapse: collapse; font-size: 11.5px; }";
    html += "th { background: #1E88E5; color: white; padding: 7px; text-align: left; }";
    html += "td { padding: 6px 7px; border-bottom: 1px solid #E2E8F0; }";
    html += "</style></head><body>";

    html += "<div class='header'>";
    html += "<div class='title'>RAPPORT STATISTIQUE & INDICATEURS RH</div>";
    html += "<div class='sub'>Institut National de la M&eacute;t&eacute;orologie &bull; Direction des Ressources Humaines</div>";
    html += QString("<div style='font-size:11px; color:#64748B; margin-top:6px;'>G&eacute;n&eacute;r&eacute; le %1 par %2 &bull; Effectif global : <b>%3 agents</b></div>")
                .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm"), editeur, QString::number(total));
    html += "</div>";

    // 1. Répartition par Poste
    html += "<div class='card'>";
    html += "<div class='card-h'>1. R&eacute;partition du Personnel par Poste M&eacute;tier</div>";
    html += "<table><tr><th>Poste Op&eacute;rationnel</th><th>Effectif</th><th>Pourcentage</th></tr>";
    QSqlQuery qp("SELECT poste, COUNT(*) FROM Employe GROUP BY poste ORDER BY COUNT(*) DESC");
    while (qp.next()) {
        int c = qp.value(1).toInt();
        int pct = qRound((c * 100.0) / total);
        html += QString("<tr><td><b>%1</b></td><td>%2</td><td>%3%</td></tr>")
                    .arg(qp.value(0).toString()).arg(c).arg(pct);
    }
    html += "</table></div>";

    // 2. Répartition par Structure
    html += "<div class='card'>";
    html += "<div class='card-h'>2. R&eacute;partition par Structure d'Affectation</div>";
    html += "<table><tr><th>Structure</th><th>Effectif</th><th>Part</th></tr>";
    QSqlQuery qs("SELECT structure, COUNT(*) FROM Employe GROUP BY structure");
    while (qs.next()) {
        int c = qs.value(1).toInt();
        int pct = qRound((c * 100.0) / total);
        html += QString("<tr><td><b>%1</b></td><td>%2</td><td>%3%</td></tr>")
                    .arg(qs.value(0).toString()).arg(c).arg(pct);
    }
    html += "</table></div>";

    // 3. Répartition par Statut
    html += "<div class='card'>";
    html += "<div class='card-h'>3. Statuts Administratifs et Disponibilit&eacute;</div>";
    html += "<table><tr><th>Statut</th><th>Effectif</th><th>Taux</th></tr>";
    QSqlQuery qst("SELECT statut, COUNT(*) FROM Employe GROUP BY statut ORDER BY COUNT(*) DESC");
    while (qst.next()) {
        int c = qst.value(1).toInt();
        int pct = qRound((c * 100.0) / total);
        html += QString("<tr><td><b>%1</b></td><td>%2</td><td>%3%</td></tr>")
                    .arg(qst.value(0).toString()).arg(c).arg(pct);
    }
    html += "</table></div>";

    html += "</body></html>";

    QTextDocument doc;
    doc.setPageSize(printer.pageLayout().paintRectPixels(printer.resolution()).size());
    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

QImage Employe::genererQrCode(const QString &texte, int taille) {
    using namespace qrcodegen;
    try {
        QrCode qr = QrCode::encodeText(texte.toUtf8().constData(), QrCode::Ecc::MEDIUM);
        const int s = qr.getSize();
        const int border = 2;
        const int totalModules = s + 2 * border;
        const int scale = qMax(1, taille / totalModules);
        const int imgSize = totalModules * scale;

        QImage img(imgSize, imgSize, QImage::Format_RGB32);
        img.fill(Qt::white);
        QPainter p(&img);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#0F172A"));

        for (int y = 0; y < s; ++y) {
            for (int x = 0; x < s; ++x) {
                if (qr.getModule(x, y)) {
                    p.fillRect((x + border) * scale, (y + border) * scale, scale, scale, QColor("#0F172A"));
                }
            }
        }
        return img;
    } catch (...) {
        QImage fallback(taille, taille, QImage::Format_RGB32);
        fallback.fill(Qt::white);
        return fallback;
    }
}

QImage Employe::genererBadgeImage(const QString &id) {
    QSqlQuery q;
    q.prepare("SELECT id_employe, nom, prenom, email, poste, structure, gouvernorat, "
              "date_recrutement, statut, type_horaire FROM employe WHERE id_employe = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) {
        QImage empty(400, 600, QImage::Format_RGB32);
        empty.fill(Qt::white);
        return empty;
    }

    QString nom = q.value(1).toString();
    QString prenom = q.value(2).toString();
    QString email = q.value(3).toString();
    QString poste = q.value(4).toString();
    QString structure = q.value(5).toString();
    QString gouv = q.value(6).toString();
    QString dateRecrut = q.value(7).toString();
    QString statut = q.value(8).toString();
    QString horaire = q.value(9).toString();

    // Dimensions format Badge / Carte RFID (500 x 780 px)
    const int W = 500;
    const int H = 780;
    QImage badge(W, H, QImage::Format_ARGB32);
    badge.fill(Qt::transparent);

    QPainter p(&badge);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // Fond de carte avec coins arrondis et bordure douce
    QRectF cardRect(10, 10, W - 20, H - 20);
    p.setPen(QPen(QColor("#CBD5E1"), 2));
    p.setBrush(Qt::white);
    p.drawRoundedRect(cardRect, 18, 18);

    // Bannière haute dégradé bleu INM
    QRectF headerRect(10, 10, W - 20, 130);
    QPainterPath headerPath;
    headerPath.addRoundedRect(cardRect, 18, 18);
    p.save();
    p.setClipPath(headerPath);
    QLinearGradient grad(0, 0, W, 130);
    grad.setColorAt(0, QColor("#0D47A1"));
    grad.setColorAt(1, QColor("#1E88E5"));
    p.fillRect(headerRect, grad);

    // Liseré rouge (Drapeau Tunisien)
    p.fillRect(QRectF(10, 136, W - 20, 5), QColor("#E53935"));
    p.restore();

    // Textes en-tête officiel
    p.setPen(Qt::white);
    QFont fHead("Arial", 8, QFont::Bold);
    fHead.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
    p.setFont(fHead);
    p.drawText(QRectF(20, 20, W - 40, 20), Qt::AlignCenter, "RÉPUBLIQUE TUNISIENNE");

    QFont fSub("Arial", 7, QFont::Normal);
    p.setFont(fSub);
    p.setPen(QColor("#BBDEFB"));
    p.drawText(QRectF(20, 38, W - 40, 16), Qt::AlignCenter, "MINISTÈRE DU TRANSPORT");

    QFont fTitle("Arial", 12, QFont::Bold);
    p.setFont(fTitle);
    p.setPen(Qt::white);
    p.drawText(QRectF(20, 56, W - 40, 26), Qt::AlignCenter, "INSTITUT NATIONAL DE LA MÉTÉOROLOGIE");

    QFont fBadge("Arial", 9, QFont::Bold);
    p.setFont(fBadge);
    p.setPen(QColor("#E0F2FE"));
    p.drawText(QRectF(20, 88, W - 40, 20), Qt::AlignCenter, "CARTE D'ACCÈS PROFESSIONNELLE");

    // Avatar / Photo cercle
    QRectF avatarRect(W / 2.0 - 50, 165, 100, 100);
    p.setPen(QPen(QColor("#1E88E5"), 4));
    p.setBrush(QColor("#F0F9FF"));
    p.drawEllipse(avatarRect);

    // Initiales au centre de l'avatar
    QString initials = (nom.left(1) + prenom.left(1)).toUpper();
    QFont fInit("Arial", 28, QFont::Bold);
    p.setFont(fInit);
    p.setPen(QColor("#0D47A1"));
    p.drawText(avatarRect, Qt::AlignCenter, initials);

    // Identité Agent
    QFont fNom("Arial", 16, QFont::Bold);
    p.setFont(fNom);
    p.setPen(QColor("#0F172A"));
    p.drawText(QRectF(20, 275, W - 40, 28), Qt::AlignCenter, QString("%1 %2").arg(nom, prenom));

    // Badge ID
    QFont fId("Arial", 10, QFont::Bold);
    p.setFont(fId);
    p.setPen(QColor("#2563EB"));
    p.drawText(QRectF(20, 305, W - 40, 20), Qt::AlignCenter, QString("MATRICULE AGENT : #%1").arg(id));

    // Badge Poste (capsule colorée)
    QRectF posteRect(50, 332, W - 100, 28);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E0F2FE"));
    p.drawRoundedRect(posteRect, 6, 6);
    QFont fPoste("Arial", 9, QFont::Bold);
    p.setFont(fPoste);
    p.setPen(QColor("#0369A1"));
    p.drawText(posteRect, Qt::AlignCenter, poste);

    // Informations affectation & structure
    QFont fInfo("Arial", 9, QFont::Normal);
    p.setFont(fInfo);
    p.setPen(QColor("#475569"));
    int yInfo = 372;
    p.drawText(QRectF(40, yInfo, W - 80, 18), Qt::AlignCenter, QString("Structure : <b>%1</b>  •  Gouvernorat : <b>%2</b>").arg(structure, gouv));
    yInfo += 20;
    p.drawText(QRectF(40, yInfo, W - 80, 18), Qt::AlignCenter, QString("Régime : %1  •  Statut : %2").arg(horaire, statut));

    // Génération et incrustation du QR Code
    QString hashSec = QString::fromLatin1(QCryptographicHash::hash((id + nom + prenom + "INM_KEY").toUtf8(), QCryptographicHash::Sha256).toHex()).left(10).toUpper();
    QString qrData = QString("INM-BADGE|ID:%1|NOM:%2|PRE:%3|POSTE:%4|GOUV:%5|SEC:%6")
                         .arg(id, nom, prenom, poste, gouv, hashSec);
    QImage qrImg = genererQrCode(qrData, 170);

    QRectF qrRect(W / 2.0 - 85, 430, 170, 170);
    p.setPen(QPen(QColor("#E2E8F0"), 1));
    p.setBrush(Qt::white);
    p.drawRoundedRect(QRectF(qrRect.left() - 6, qrRect.top() - 6, 182, 182), 8, 8);
    p.drawImage(qrRect, qrImg);

    // Puce RFID / Sécurité virtuelle
    QFont fSec("Arial", 8, QFont::Bold);
    p.setFont(fSec);
    p.setPen(QColor("#64748B"));
    p.drawText(QRectF(20, 625, W - 40, 18), Qt::AlignCenter, QString("AUTHENTIFICATION NUMÉRIQUE • CLÉ : %1").arg(hashSec));

    // Pied de badge
    p.setPen(QPen(QColor("#E2E8F0"), 1));
    p.drawLine(40, 655, W - 40, 655);

    QFont fFoot("Arial", 7.5, QFont::Normal);
    p.setFont(fFoot);
    p.setPen(QColor("#94A3B8"));
    p.drawText(QRectF(20, 665, W - 40, 16), Qt::AlignCenter, "Carte strictement personnelle • Propriété de l'INM");
    p.drawText(QRectF(20, 683, W - 40, 16), Qt::AlignCenter, "En cas de perte, veuillez contacter le service RH (inm.tn)");

    return badge;
}

bool Employe::exporterBadgePDF(const QString &id, const QString &cheminFichier) {
    QImage badge = genererBadgeImage(id);
    if (badge.isNull()) return false;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageOrientation(QPageLayout::Portrait);

    QPainter painter(&printer);
    QRect pageRect = printer.pageLayout().paintRectPixels(printer.resolution());
    
    // Centrer le badge sur la page PDF
    int bw = pageRect.width() * 0.65;
    int bh = (bw * badge.height()) / badge.width();
    int bx = (pageRect.width() - bw) / 2;
    int by = (pageRect.height() - bh) / 2;

    painter.drawImage(QRect(bx, by, bw, bh), badge);
    painter.end();
    return true;
}

bool Employe::logAction(const QString &action, const QString &details, const QString &utilisateur) {
    // S'assurer de la présence de la table historique_rh
    QSqlQuery check("SELECT 1 FROM historique_rh WHERE 1 = 0");
    if (!check.isActive()) {
        QSqlQuery create("CREATE TABLE historique_rh ("
                         "id_log NUMBER GENERATED ALWAYS AS IDENTITY, "
                         "date_action VARCHAR2(30), "
                         "utilisateur VARCHAR2(50), "
                         "action VARCHAR2(100), "
                         "details VARCHAR2(255))");
        if (!create.isActive()) {
            QSqlQuery createSqlite("CREATE TABLE IF NOT EXISTS historique_rh ("
                                   "id_log INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "date_action TEXT, "
                                   "utilisateur TEXT, "
                                   "action TEXT, "
                                   "details TEXT)");
            createSqlite.exec();
        }
    }

    QSqlQuery q;
    q.prepare("INSERT INTO historique_rh (date_action, utilisateur, action, details) VALUES (:d, :u, :a, :det)");
    q.bindValue(":d", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":u", utilisateur.isEmpty() ? "admin" : utilisateur);
    q.bindValue(":a", action);
    q.bindValue(":det", details);
    return q.exec();
}

QSqlQueryModel* Employe::afficherLogs() {
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT date_action, utilisateur, action, details FROM historique_rh ORDER BY date_action DESC");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Date & Heure"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Utilisateur"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Action Métier"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Détails de l'Opération"));
    return model;
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
    else if (critere == "Prénom") col = "Prenom";
    else if (critere == "Poste") col = "Poste";
    else if (critere == "Date de recrutement") col = "Date_Recrutement";
    else if (critere == "Structure") col = "Structure";
    else if (critere == "Gouvernorat") col = "Gouvernorat";
    else if (critere == "Statut") col = "Statut";
    else if (critere == "ID") col = "ID_Employe";
    else if (critere == "Horaire") col = "Type_Horaire";

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

QSqlQueryModel* Employe::rechercher(const QString &texte, const QString &critere, const QString &tri, const QString &ordre) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QString colTri = "Nom";
    if (tri == "Poste") colTri = "Poste";
    else if (tri == "Prénom") colTri = "Prenom";
    else if (tri == "Date de recrutement") colTri = "Date_Recrutement";
    else if (tri == "Structure") colTri = "Structure";
    else if (tri == "Gouvernorat") colTri = "Gouvernorat";
    else if (tri == "ID") colTri = "ID_Employe";
    else if (tri == "Statut") colTri = "Statut";

    QString dir = (ordre.toUpper() == "DESC") ? "DESC" : "ASC";

    QSqlQuery query;
    if (critere == "Tous" || critere.isEmpty()) {
        query.prepare(QString("SELECT ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire "
                              "FROM Employe WHERE (UPPER(Nom) LIKE UPPER(:val) OR UPPER(Prenom) LIKE UPPER(:val) "
                              "OR UPPER(Email) LIKE UPPER(:val) OR UPPER(Poste) LIKE UPPER(:val) "
                              "OR UPPER(Gouvernorat) LIKE UPPER(:val) OR UPPER(Structure) LIKE UPPER(:val) "
                              "OR UPPER(Statut) LIKE UPPER(:val) OR UPPER(ID_Employe) LIKE UPPER(:val)) "
                              "ORDER BY %1 %2").arg(colTri, dir));
    } else {
        QString colRecherche = "Nom";
        if (critere == "Prénom") colRecherche = "Prenom";
        else if (critere == "Poste") colRecherche = "Poste";
        else if (critere == "Structure") colRecherche = "Structure";
        else if (critere == "Gouvernorat") colRecherche = "Gouvernorat";
        else if (critere == "ID") colRecherche = "ID_Employe";
        else if (critere == "Email") colRecherche = "Email";
        else if (critere == "Statut") colRecherche = "Statut";

        query.prepare(QString("SELECT ID_Employe, Nom, Prenom, Email, Poste, Structure, Gouvernorat, Date_Recrutement, Statut, Type_Horaire "
                              "FROM Employe WHERE UPPER(%1) LIKE UPPER(:val) ORDER BY %2 %3").arg(colRecherche, colTri, dir));
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