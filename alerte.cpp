#include "alerte.h"
#include "dbcompat.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QPrinter>
#include <QPainter>

Alerte::Alerte()
    : statut("En cours")
{
}

Alerte::Alerte(const QString &id, const QString &type, const QString &severite,
               const QDateTime &emission, const QDateTime &fin,
               const QString &station, const QString &gouv,
               const QString &statut, const QString &desc)
    : idAlerte(id), typeAlerte(type), niveauSeverite(severite),
      dateEmission(emission), dateFin(fin),
      idStation(station), gouvernorat(gouv),
      statut(statut), description(desc)
{
}

bool Alerte::existe(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM ALERTES WHERE ID_ALERTE = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Alerte::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO ALERTES (ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION) "
                  "VALUES (:id, :type, :sev, :demis, :dfin, :st, :gouv, :statut, :desc)");
    query.bindValue(":id", idAlerte);
    query.bindValue(":type", typeAlerte);
    query.bindValue(":sev", niveauSeverite);
    query.bindValue(":demis", dateEmission.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":dfin", dateFin.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":st", idStation);
    query.bindValue(":gouv", gouvernorat);
    query.bindValue(":statut", statut);
    query.bindValue(":desc", description);

    if (!query.exec()) {
        qWarning() << "Erreur ajout alerte:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Alerte::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE ALERTES SET TYPE_ALERTE = :type, NIVEAU_SEVERITE = :sev, "
                  "DATE_EMISSION = :demis, DATE_FIN = :dfin, ID_STATION = :st, "
                  "GOUVERNORAT = :gouv, STATUT = :statut, DESCRIPTION = :desc "
                  "WHERE ID_ALERTE = :id");
    query.bindValue(":id", idAlerte);
    query.bindValue(":type", typeAlerte);
    query.bindValue(":sev", niveauSeverite);
    query.bindValue(":demis", dateEmission.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":dfin", dateFin.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":st", idStation);
    query.bindValue(":gouv", gouvernorat);
    query.bindValue(":statut", statut);
    query.bindValue(":desc", description);

    if (!query.exec()) {
        qWarning() << "Erreur modification alerte:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Alerte::mettreAJourSeverite(const QString &id, const QString &nouveauNiveau)
{
    QSqlQuery query;
    query.prepare("UPDATE ALERTES SET NIVEAU_SEVERITE = :sev WHERE ID_ALERTE = :id");
    query.bindValue(":sev", nouveauNiveau);
    query.bindValue(":id", id);
    return query.exec();
}

bool Alerte::supprimer(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM ALERTES WHERE ID_ALERTE = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool Alerte::archiver(const QString &id)
{
    QSqlQuery query;
    query.prepare("UPDATE ALERTES SET STATUT = 'Archivée' WHERE ID_ALERTE = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QSqlQueryModel* Alerte::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION FROM ALERTES ORDER BY DATE_EMISSION DESC");

    model->setHeaderData(0, Qt::Horizontal, "ID Alerte");
    model->setHeaderData(1, Qt::Horizontal, "Type d'Alerte");
    model->setHeaderData(2, Qt::Horizontal, "Sévérité");
    model->setHeaderData(3, Qt::Horizontal, "Date Émission");
    model->setHeaderData(4, Qt::Horizontal, "Date Fin");
    model->setHeaderData(5, Qt::Horizontal, "Station Réf.");
    model->setHeaderData(6, Qt::Horizontal, "Gouvernorat");
    model->setHeaderData(7, Qt::Horizontal, "Statut");
    model->setHeaderData(8, Qt::Horizontal, "Description");

    return model;
}

QSqlQueryModel* Alerte::rechercher(const QString &terme, const QString &typeFiltre, const QString &severiteFiltre, const QString &statutFiltre)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QString queryString = "SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION FROM ALERTES WHERE 1=1";

    if (!terme.trimmed().isEmpty()) {
        queryString += QString(" AND (ID_ALERTE LIKE '%%1%' OR GOUVERNORAT LIKE '%%1%' OR ID_STATION LIKE '%%1%' OR DESCRIPTION LIKE '%%1%')").arg(terme.trimmed());
    }
    if (typeFiltre != "Tous" && !typeFiltre.isEmpty()) {
        queryString += QString(" AND TYPE_ALERTE = '%1'").arg(typeFiltre);
    }
    if (severiteFiltre != "Tous" && !severiteFiltre.isEmpty()) {
        queryString += QString(" AND NIVEAU_SEVERITE = '%1'").arg(severiteFiltre);
    }
    if (statutFiltre != "Tous" && !statutFiltre.isEmpty()) {
        queryString += QString(" AND STATUT = '%1'").arg(statutFiltre);
    }

    queryString += " ORDER BY DATE_EMISSION DESC";
    model->setQuery(queryString);

    model->setHeaderData(0, Qt::Horizontal, "ID Alerte");
    model->setHeaderData(1, Qt::Horizontal, "Type d'Alerte");
    model->setHeaderData(2, Qt::Horizontal, "Sévérité");
    model->setHeaderData(3, Qt::Horizontal, "Date Émission");
    model->setHeaderData(4, Qt::Horizontal, "Date Fin");
    model->setHeaderData(5, Qt::Horizontal, "Station Réf.");
    model->setHeaderData(6, Qt::Horizontal, "Gouvernorat");
    model->setHeaderData(7, Qt::Horizontal, "Statut");
    model->setHeaderData(8, Qt::Horizontal, "Description");

    return model;
}

QSqlQueryModel* Alerte::trier(const QString &critere, const QString &ordre)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QString queryString;

    if (critere == "severite") {
        // Ordre logique métier: Rouge > Orange > Jaune
        if (ordre.toUpper() == "DESC" || ordre.toUpper() == "ROUGE_FIRST") {
            queryString = "SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION "
                          "FROM ALERTES ORDER BY CASE NIVEAU_SEVERITE "
                          "WHEN 'Rouge' THEN 1 WHEN 'Orange' THEN 2 WHEN 'Jaune' THEN 3 ELSE 4 END ASC, DATE_EMISSION DESC";
        } else {
            queryString = "SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION "
                          "FROM ALERTES ORDER BY CASE NIVEAU_SEVERITE "
                          "WHEN 'Jaune' THEN 1 WHEN 'Orange' THEN 2 WHEN 'Rouge' THEN 3 ELSE 4 END ASC, DATE_EMISSION DESC";
        }
    } else if (critere == "date") {
        queryString = QString("SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION "
                              "FROM ALERTES ORDER BY DATE_EMISSION %1").arg(ordre);
    } else {
        queryString = "SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION FROM ALERTES ORDER BY DATE_EMISSION DESC";
    }

    model->setQuery(queryString);
    model->setHeaderData(0, Qt::Horizontal, "ID Alerte");
    model->setHeaderData(1, Qt::Horizontal, "Type d'Alerte");
    model->setHeaderData(2, Qt::Horizontal, "Sévérité");
    model->setHeaderData(3, Qt::Horizontal, "Date Émission");
    model->setHeaderData(4, Qt::Horizontal, "Date Fin");
    model->setHeaderData(5, Qt::Horizontal, "Station Réf.");
    model->setHeaderData(6, Qt::Horizontal, "Gouvernorat");
    model->setHeaderData(7, Qt::Horizontal, "Statut");
    model->setHeaderData(8, Qt::Horizontal, "Description");

    return model;
}

bool Alerte::exporterTXT(const QString &cheminFichier)
{
    QFile file(cheminFichier);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "========================================================================================\n";
    out << "       SMART WEATHER MANAGEMENT - JOURNAL OFFICIEL DES ALERTES METEOROLOGIQUES\n";
    out << "       Module 4 : Gestion des Alertes & Predictions\n";
    out << "       Date d'exportation : " << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << "\n";
    out << "========================================================================================\n\n";

    QSqlQuery query("SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION FROM ALERTES ORDER BY DATE_EMISSION DESC");

    int count = 0;
    while (query.next()) {
        count++;
        out << QString("[%1] %2\n").arg(count, 3, 10, QChar('0')).arg(query.value(0).toString());
        out << "   * Type de Risque   : " << query.value(1).toString() << "\n";
        out << "   * Niveau Severite  : " << query.value(2).toString() << "\n";
        out << "   * Periode          : du " << query.value(3).toString() << " au " << query.value(4).toString() << "\n";
        out << "   * Station / Region : " << query.value(5).toString() << " (" << query.value(6).toString() << ")\n";
        out << "   * Statut Operation : " << query.value(7).toString() << "\n";
        out << "   * Description      : " << query.value(8).toString() << "\n";
        out << "----------------------------------------------------------------------------------------\n";
    }

    out << "\nTotal des alertes exportees : " << count << "\n";
    out << "Document certifie conforme pour archivage reglementaire.\n";
    file.close();
    return true;
}

bool Alerte::exporterPDF(const QString &cheminFichier, const QString &utilisateur)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageOrientation(QPageLayout::Landscape);

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'/>";
    html += "<style>";
    html += "body { font-family: 'Helvetica Neue', Arial, sans-serif; margin: 20px; color: #2c3e50; }";
    html += ".header { text-align: center; border-bottom: 3px solid #2980b9; padding-bottom: 12px; margin-bottom: 20px; }";
    html += ".title { font-size: 24px; font-weight: bold; color: #1a365d; margin: 0; }";
    html += ".subtitle { font-size: 14px; color: #4a5568; margin-top: 5px; }";
    html += ".meta-box { background: #edf2f7; border-radius: 6px; padding: 10px; margin-bottom: 15px; font-size: 12px; }";
    html += "table { width: 100%; border-collapse: collapse; margin-top: 10px; font-size: 11px; }";
    html += "th { background: #2b6cb0; color: white; padding: 8px 6px; text-align: left; }";
    html += "td { padding: 7px 6px; border-bottom: 1px solid #e2e8f0; }";
    html += "tr:nth-child(even) { background-color: #f7fafc; }";
    html += ".badge-rouge { background-color: #e53e3e; color: white; padding: 3px 7px; border-radius: 4px; font-weight: bold; }";
    html += ".badge-orange { background-color: #dd6b20; color: white; padding: 3px 7px; border-radius: 4px; font-weight: bold; }";
    html += ".badge-jaune { background-color: #d69e2e; color: white; padding: 3px 7px; border-radius: 4px; font-weight: bold; }";
    html += ".footer { margin-top: 25px; text-align: right; font-size: 10px; color: #718096; border-top: 1px solid #cbd5e0; padding-top: 8px; }";
    html += "</style></head><body>";

    html += "<div class='header'>";
    html += "<div class='title'>SMART WEATHER MANAGEMENT</div>";
    html += "<div class='subtitle'>Direction de la Vigilance & des Alertes Météorologiques — Module 4</div>";
    html += "</div>";

    html += "<div class='meta-box'>";
    html += "<b>Date d'édition :</b> " + QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm:ss") + " | ";
    html += "<b>Émis par :</b> " + utilisateur + " | ";
    html += "<b>Usage :</b> Archivage réglementaire & Protection Civile";
    html += "</div>";

    html += "<table>";
    html += "<thead><tr>";
    html += "<th>ID Alerte</th>";
    html += "<th>Type</th>";
    html += "<th>Sévérité</th>";
    html += "<th>Date Début</th>";
    html += "<th>Date Fin</th>";
    html += "<th>Station</th>";
    html += "<th>Gouvernorat</th>";
    html += "<th>Statut</th>";
    html += "<th>Description</th>";
    html += "</tr></thead><tbody>";

    QSqlQuery query("SELECT ID_ALERTE, TYPE_ALERTE, NIVEAU_SEVERITE, DATE_EMISSION, DATE_FIN, ID_STATION, GOUVERNORAT, STATUT, DESCRIPTION FROM ALERTES ORDER BY DATE_EMISSION DESC");

    int rowCount = 0;
    while (query.next()) {
        rowCount++;
        QString sev = query.value(2).toString();
        QString badgeClass = "badge-jaune";
        if (sev.compare("Rouge", Qt::CaseInsensitive) == 0) badgeClass = "badge-rouge";
        else if (sev.compare("Orange", Qt::CaseInsensitive) == 0) badgeClass = "badge-orange";

        html += "<tr>";
        html += QString("<td><b>%1</b></td>").arg(query.value(0).toString());
        html += QString("<td>%1</td>").arg(query.value(1).toString());
        html += QString("<td><span class='%1'>%2</span></td>").arg(badgeClass, sev);
        html += QString("<td>%1</td>").arg(query.value(3).toString());
        html += QString("<td>%1</td>").arg(query.value(4).toString());
        html += QString("<td>%1</td>").arg(query.value(5).toString());
        html += QString("<td>%1</td>").arg(query.value(6).toString());
        html += QString("<td>%1</td>").arg(query.value(7).toString());
        html += QString("<td>%1</td>").arg(query.value(8).toString());
        html += "</tr>";
    }

    html += "</tbody></table>";
    html += QString("<div class='footer'>Rapport officiel généré automatiquement par Smart Weather Management System (SWM) - Total alertes : %1</div>").arg(rowCount);
    html += "</body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    return true;
}

QMap<QString, int> Alerte::statistiquesParGouvernorat(int annee)
{
    QMap<QString, int> stats;
    QSqlQuery query;
    // TO_CHAR sous Oracle, strftime sous SQLite (voir DbCompat::dateExpr).
    query.prepare("SELECT GOUVERNORAT, COUNT(*) FROM ALERTES WHERE "
                  + DbCompat::dateExpr("DATE_EMISSION", "YYYY") + " = :annee "
                  "GROUP BY GOUVERNORAT ORDER BY COUNT(*) DESC");
    query.bindValue(":annee", QString::number(annee));

    if (query.exec()) {
        while (query.next()) {
            stats.insert(query.value(0).toString(), query.value(1).toInt());
        }
    }

    // Si aucune alerte sur l'année spécifique, fallback global
    if (stats.isEmpty()) {
        QSqlQuery fallback("SELECT GOUVERNORAT, COUNT(*) FROM ALERTES GROUP BY GOUVERNORAT ORDER BY COUNT(*) DESC");
        while (fallback.next()) {
            stats.insert(fallback.value(0).toString(), fallback.value(1).toInt());
        }
    }

    return stats;
}

QMap<QString, int> Alerte::statistiquesParSeverite()
{
    QMap<QString, int> stats;
    QSqlQuery query("SELECT NIVEAU_SEVERITE, COUNT(*) FROM ALERTES GROUP BY NIVEAU_SEVERITE");
    while (query.next()) {
        stats.insert(query.value(0).toString(), query.value(1).toInt());
    }
    return stats;
}

QMap<QString, int> Alerte::statistiquesParType()
{
    QMap<QString, int> stats;
    QSqlQuery query("SELECT TYPE_ALERTE, COUNT(*) FROM ALERTES GROUP BY TYPE_ALERTE");
    while (query.next()) {
        stats.insert(query.value(0).toString(), query.value(1).toInt());
    }
    return stats;
}

int Alerte::compterAlertesEnCours()
{
    QSqlQuery query("SELECT COUNT(*) FROM ALERTES WHERE STATUT = 'En cours'");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

int Alerte::compterAlertesCritiques()
{
    QSqlQuery query("SELECT COUNT(*) FROM ALERTES WHERE STATUT = 'En cours' AND NIVEAU_SEVERITE = 'Rouge'");
    if (query.next()) return query.value(0).toInt();
    return 0;
}
