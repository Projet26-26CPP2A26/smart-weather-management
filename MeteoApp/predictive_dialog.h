#pragma once
#include <QDate>
#include <QDialog>
#include <QHeaderView>
#include <QLabel>
#include <QSqlQuery>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVector>

#include "maintenance_db.h"

// Planification preventive predictive.
// Analyse l'evolution du Heartbeat Score (module 2) ET les anomalies capteur
// non resolues (module 3) pour proposer une intervention preventive avant la panne,
// avec une priorite et une date suggerees.
// Double-clic sur une ligne => signal planRequested(...)
class PredictiveDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PredictiveDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Planification préventive prédictive");
        resize(780, 380);

        auto *v = new QVBoxLayout(this);
        v->addWidget(new QLabel("Stations à risque (Heartbeat Score en baisse et/ou anomalies capteur). "
                                "Double-cliquez pour planifier :"));

        m_table = new QTableWidget(0, 6);
        m_table->setHorizontalHeaderLabels({"Station", "Score", "Tendance", "Anomalies capteur",
                                            "Priorité suggérée", "Date suggérée"});
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->horizontalHeader()->setStretchLastSection(true);
        m_table->verticalHeader()->hide();
        v->addWidget(m_table);

        // Dernier score + score precedent (tendance) + anomalies non resolues.
        // Les stations ayant deja une intervention preventive ouverte sont ignorees.
        // Score Heartbeat (V_HEARTBEAT, module 2) + tendance + anomalies capteur recentes
        // (V_ANOMALIES_RECENTES, releves du module 3).
        // Les stations ayant deja une intervention preventive ouverte sont ignorees.
        QSqlQuery q(dbSql(
            "SELECT * FROM ("
            "  SELECT s.ID_STATION, s.NOM, "
            "    NVL((SELECT MAX(h.SCORE) FROM V_HEARTBEAT h WHERE h.ID_STATION = s.ID_STATION "
            "         AND h.DATE_MESURE = (SELECT MAX(DATE_MESURE) FROM V_HEARTBEAT WHERE ID_STATION = s.ID_STATION)), 100) AS SCORE, "
            "    (SELECT AVG(h2.SCORE) FROM V_HEARTBEAT h2 WHERE h2.ID_STATION = s.ID_STATION "
            "         AND h2.DATE_MESURE < (SELECT MAX(DATE_MESURE) FROM V_HEARTBEAT WHERE ID_STATION = s.ID_STATION)) AS PREV, "
            "    NVL((SELECT SUM(a.NB_ANO)  FROM V_ANOMALIES_RECENTES a WHERE a.ID_STATION = s.ID_STATION), 0) AS NB_ANO, "
            "    NVL((SELECT SUM(a.NB_CRIT) FROM V_ANOMALIES_RECENTES a WHERE a.ID_STATION = s.ID_STATION), 0) AS NB_CRIT "
            "  FROM STATION s "
            "  WHERE NOT EXISTS (SELECT 1 FROM INTERVENTION i WHERE i.ID_STATION = s.ID_STATION "
            "         AND i.TYPE_INTERVENTION = 'Préventive' AND i.STATUT IN ('Planifiée','En cours') AND i.ARCHIVE = 0)"
            ") WHERE SCORE < 75 OR NB_ANO > 0 ORDER BY SCORE"));

        while (q.next()) {
            const double score = q.value(2).toDouble();
            const bool hasPrev = !q.value(3).isNull();
            const double prev = q.value(3).toDouble();
            const int nbAno = q.value(4).toInt();
            const int nbCrit = q.value(5).toInt();

            QString trend = "–";
            if (hasPrev) trend = (score < prev - 1) ? "↓ en baisse" : (score > prev + 1 ? "↑ en hausse" : "→ stable");

            QString prio; int days;
            if (score < 40 || nbCrit > 0)      { prio = "Urgente"; days = 1; }
            else if (score < 60 || nbAno >= 2) { prio = "Haute";   days = 3; }
            else                               { prio = "Moyenne"; days = 7; }
            // Score en chute rapide : on avance la date d'un cran
            if (hasPrev && score < prev - 10 && days > 1) days = qMax(1, days / 2);

            const QDate d = QDate::currentDate().addDays(days);
            const QString note = QString("Intervention préventive suggérée automatiquement — Heartbeat Score : %1 (%2), "
                                         "anomalies capteur non résolues : %3 dont %4 critique(s).")
                                     .arg(score, 0, 'f', 1).arg(trend).arg(nbAno).arg(nbCrit);
            m_sugs.push_back({q.value(0).toInt(), prio, d, note});

            const int row = m_table->rowCount();
            m_table->insertRow(row);
            m_table->setItem(row, 0, new QTableWidgetItem(q.value(1).toString()));
            m_table->setItem(row, 1, new QTableWidgetItem(QString::number(score, 'f', 1)));
            m_table->setItem(row, 2, new QTableWidgetItem(trend));
            m_table->setItem(row, 3, new QTableWidgetItem(QString::number(nbAno)));
            m_table->setItem(row, 4, new QTableWidgetItem(prio));
            m_table->setItem(row, 5, new QTableWidgetItem(d.toString("dd/MM/yyyy")));
        }

        connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int) {
            emit planRequested(m_sugs[row].id, m_sugs[row].prio, m_sugs[row].date, m_sugs[row].note);
        });
    }

    bool isEmpty() const { return m_sugs.isEmpty(); }

signals:
    void planRequested(int idStation, const QString &priorite, const QDate &date, const QString &note);

private:
    struct Sug { int id; QString prio; QDate date; QString note; };
    QVector<Sug> m_sugs;
    QTableWidget *m_table;
};
