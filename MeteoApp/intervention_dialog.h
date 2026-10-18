#pragma once
#include <QDate>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>

#include "ui_intervention_dialog.h"
#include "maintenance_db.h"

// Dialogue d'ajout / modification d'une intervention.
// id == 0  -> creation   |   id > 0 -> modification
class InterventionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InterventionDialog(int id = 0, QWidget *parent = nullptr)
        : QDialog(parent), ui(new Ui::InterventionDialog), m_id(id)
    {
        ui->setupUi(this);
        setWindowTitle(id ? "Modifier l'intervention" : "Nouvelle intervention");
        ui->dlgTitle->setText(id ? QString("✎  Modifier l'intervention n°%1").arg(id)
                                 : QString("🛠  Nouvelle intervention de maintenance"));
        setMinimumWidth(600);

        ui->dePlan->setDate(QDate::currentDate());
        ui->spCout->setRange(0, 9999999);
        ui->spCout->setDecimals(2);
        ui->spCout->setSuffix(" DT");

        QPushButton *save = ui->buttonBox->button(QDialogButtonBox::Save);
        save->setText("💾  Enregistrer");
        save->setProperty("role", "primary");
        QPushButton *cancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
        cancel->setText("Annuler");
        save->setCursor(Qt::PointingHandCursor);
        cancel->setCursor(Qt::PointingHandCursor);
        connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        loadCombos();
        if (m_id) loadExisting();
    }

    ~InterventionDialog() override { delete ui; }

    // Pre-remplissage (ex: depuis la planification predictive)
    void preset(int idStation, const QString &type, const QString &priorite, const QDate &datePlanifiee,
                const QString &note = QString())
    {
        ui->cbStation->setCurrentIndex(ui->cbStation->findData(idStation));
        ui->cbType->setCurrentText(type);
        ui->cbPriorite->setCurrentText(priorite);
        ui->dePlan->setDate(datePlanifiee);
        if (!note.isEmpty()) ui->teRapport->setPlainText(note);
    }

protected:
    void accept() override
    {
        if (ui->cbStation->currentIndex() < 0 || ui->cbEmploye->currentIndex() < 0) {
            QMessageBox::warning(this, "Champs manquants", "Station et technicien sont obligatoires.");
            return;
        }
        // Nouvelle intervention : la date planifiee ne peut pas etre dans le passe
        if (m_id == 0 && ui->dePlan->date() < QDate::currentDate()) {
            QMessageBox::warning(this, "Date invalide", "La date planifiée ne peut pas être dans le passé.");
            return;
        }
        if (ui->dePlan->date() < QDate::currentDate().addYears(-5)) {
            QMessageBox::warning(this, "Date invalide", "La date planifiée est incorrecte.");
            return;
        }
        if (ui->cbStatut->currentText() == "Terminée"
            && ui->teRapport->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Rapport requis",
                                 "Un rapport est obligatoire pour clôturer une intervention.");
            return;
        }

        QSqlQuery q;
        if (m_id == 0) {
            q.prepare(dbSql("INSERT INTO INTERVENTION (ID_INTERVENTION, ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, "
                      "PRIORITE, DATE_CREATION, DATE_PLANIFIEE, DATE_CLOTURE, STATUT, COUT, RAPPORT_INTERVENTION) "
                      "VALUES (SEQ_INTERVENTION.NEXTVAL, ?, ?, ?, ?, SYSDATE, ?, "
                      "CASE WHEN ? = 'Terminée' THEN SYSDATE ELSE NULL END, ?, ?, ?)"));
        } else {
            q.prepare(dbSql("UPDATE INTERVENTION SET ID_STATION=?, ID_EMPLOYE=?, TYPE_INTERVENTION=?, PRIORITE=?, "
                      "DATE_PLANIFIEE=?, "
                      "DATE_CLOTURE = CASE WHEN ? = 'Terminée' THEN NVL(DATE_CLOTURE, SYSDATE) ELSE NULL END, "
                      "STATUT=?, COUT=?, RAPPORT_INTERVENTION=? WHERE ID_INTERVENTION=?"));
        }
        q.addBindValue(ui->cbStation->currentData());
        q.addBindValue(ui->cbEmploye->currentData());
        q.addBindValue(ui->cbType->currentText());
        q.addBindValue(ui->cbPriorite->currentText());
        q.addBindValue(ui->dePlan->dateTime());
        q.addBindValue(ui->cbStatut->currentText());   // pour le CASE
        q.addBindValue(ui->cbStatut->currentText());
        q.addBindValue(ui->spCout->value());
        q.addBindValue(ui->teRapport->toPlainText());
        if (m_id) q.addBindValue(m_id);

        if (!q.exec()) {
            QMessageBox::critical(this, "Erreur Oracle", q.lastError().text());
            return;
        }
        QDialog::accept();
    }

private:
    void loadCombos()
    {
        QSqlQuery q("SELECT ID_STATION, NOM FROM STATION ORDER BY NOM");
        while (q.next())
            ui->cbStation->addItem(q.value(1).toString(), q.value(0));

        QSqlQuery e("SELECT ID_EMPLOYE, NOM || ' ' || PRENOM FROM EMPLOYE ORDER BY NOM");
        while (e.next())
            ui->cbEmploye->addItem(e.value(1).toString(), e.value(0));
    }

    void loadExisting()
    {
        QSqlQuery q;
        q.prepare("SELECT ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, PRIORITE, DATE_PLANIFIEE, "
                  "STATUT, COUT, RAPPORT_INTERVENTION FROM INTERVENTION WHERE ID_INTERVENTION = ?");
        q.addBindValue(m_id);
        if (!q.exec() || !q.next()) return;

        ui->cbStation->setCurrentIndex(ui->cbStation->findData(q.value(0)));
        ui->cbEmploye->setCurrentIndex(ui->cbEmploye->findData(q.value(1)));
        ui->cbType->setCurrentText(q.value(2).toString());
        ui->cbPriorite->setCurrentText(q.value(3).toString());
        if (!q.value(4).isNull()) ui->dePlan->setDate(q.value(4).toDateTime().date());
        ui->cbStatut->setCurrentText(q.value(5).toString());
        ui->spCout->setValue(q.value(6).toDouble());
        ui->teRapport->setPlainText(q.value(7).toString());
    }

    Ui::InterventionDialog *ui;
    int m_id;
};
