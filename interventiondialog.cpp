#include "interventiondialog.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextEdit>
#include <QVBoxLayout>

InterventionDialog::InterventionDialog(int id, QWidget *parent)
    : QDialog(parent), m_id(id)
{
    setWindowTitle(id ? "Modifier l'intervention" : "Nouvelle intervention");
    setMinimumWidth(460);

    cbStation  = new QComboBox;
    cbEmploye  = new QComboBox;
    cbType     = new QComboBox;
    cbPriorite = new QComboBox;
    cbStatut   = new QComboBox;
    dePlan     = new QDateEdit(QDate::currentDate());
    spCout     = new QDoubleSpinBox;
    teRapport  = new QTextEdit;

    cbType->addItems({"Préventive", "Corrective", "Calibration", "Remplacement capteur"});
    cbPriorite->addItems({"Basse", "Moyenne", "Haute", "Urgente"});
    cbStatut->addItems({"Planifiée", "En cours", "Terminée", "Annulée"});
    dePlan->setCalendarPopup(true);
    dePlan->setDisplayFormat("dd/MM/yyyy");
    spCout->setRange(0, 9999999);
    spCout->setDecimals(2);
    spCout->setSuffix(" DT");
    teRapport->setPlaceholderText("Rapport d'intervention...");
    teRapport->setFixedHeight(110);

    auto *form = new QFormLayout;
    form->setSpacing(10);
    form->addRow("Station *", cbStation);
    form->addRow("Technicien *", cbEmploye);
    form->addRow("Type *", cbType);
    form->addRow("Priorité *", cbPriorite);
    form->addRow("Date planifiée", dePlan);
    form->addRow("Statut", cbStatut);
    form->addRow("Coût", spCout);
    form->addRow("Rapport", teRapport);

    auto *box = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    box->button(QDialogButtonBox::Save)->setObjectName("primary");
    box->button(QDialogButtonBox::Save)->setText("Enregistrer");
    box->button(QDialogButtonBox::Cancel)->setText("Annuler");
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(20, 20, 20, 20);
    lay->addLayout(form);
    lay->addSpacing(8);
    lay->addWidget(box);

    loadCombos();
    if (m_id) loadExisting();
}

void InterventionDialog::loadCombos()
{
    QSqlQuery q("SELECT ID_STATION, NOM FROM STATION ORDER BY NOM");
    while (q.next())
        cbStation->addItem(q.value(1).toString(), q.value(0));

    QSqlQuery e("SELECT ID_EMPLOYE, NOM || ' ' || PRENOM FROM EMPLOYE ORDER BY NOM");
    while (e.next())
        cbEmploye->addItem(e.value(1).toString(), e.value(0));
}

void InterventionDialog::loadExisting()
{
    QSqlQuery q;
    q.prepare("SELECT ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, PRIORITE, DATE_PLANIFIEE, "
              "STATUT, COUT, RAPPORT_INTERVENTION FROM INTERVENTION WHERE ID_INTERVENTION = ?");
    q.addBindValue(m_id);
    if (!q.exec() || !q.next()) return;

    cbStation->setCurrentIndex(cbStation->findData(q.value(0)));
    cbEmploye->setCurrentIndex(cbEmploye->findData(q.value(1)));
    cbType->setCurrentText(q.value(2).toString());
    cbPriorite->setCurrentText(q.value(3).toString());
    if (!q.value(4).isNull()) dePlan->setDate(q.value(4).toDate());
    cbStatut->setCurrentText(q.value(5).toString());
    spCout->setValue(q.value(6).toDouble());
    teRapport->setPlainText(q.value(7).toString());
}

void InterventionDialog::preset(int idStation, const QString &type,
                                const QString &priorite, const QDate &datePlanifiee)
{
    cbStation->setCurrentIndex(cbStation->findData(idStation));
    cbType->setCurrentText(type);
    cbPriorite->setCurrentText(priorite);
    dePlan->setDate(datePlanifiee);
}

void InterventionDialog::accept()
{
    if (cbStation->currentIndex() < 0 || cbEmploye->currentIndex() < 0) {
        QMessageBox::warning(this, "Champs manquants", "Station et technicien sont obligatoires.");
        return;
    }
    if (dePlan->date() < QDate::currentDate().addYears(-5)) {
        QMessageBox::warning(this, "Date invalide", "La date planifiée est incorrecte.");
        return;
    }
    if (cbStatut->currentText() == "Terminée" && teRapport->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Rapport requis",
                             "Un rapport est obligatoire pour clôturer une intervention.");
        return;
    }

    QSqlQuery q;
    if (m_id == 0) {
        q.prepare("INSERT INTO INTERVENTION (ID_INTERVENTION, ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, "
                  "PRIORITE, DATE_CREATION, DATE_PLANIFIEE, DATE_CLOTURE, STATUT, COUT, RAPPORT_INTERVENTION) "
                  "VALUES (SEQ_INTERVENTION.NEXTVAL, ?, ?, ?, ?, SYSDATE, ?, "
                  "CASE WHEN ? = 'Terminée' THEN SYSDATE ELSE NULL END, ?, ?, ?)");
    } else {
        q.prepare("UPDATE INTERVENTION SET ID_STATION=?, ID_EMPLOYE=?, TYPE_INTERVENTION=?, PRIORITE=?, "
                  "DATE_PLANIFIEE=?, "
                  "DATE_CLOTURE = CASE WHEN ? = 'Terminée' THEN NVL(DATE_CLOTURE, SYSDATE) ELSE NULL END, "
                  "STATUT=?, COUT=?, RAPPORT_INTERVENTION=? WHERE ID_INTERVENTION=?");
    }
    q.addBindValue(cbStation->currentData());
    q.addBindValue(cbEmploye->currentData());
    q.addBindValue(cbType->currentText());
    q.addBindValue(cbPriorite->currentText());
    q.addBindValue(dePlan->dateTime());
    q.addBindValue(cbStatut->currentText());   // pour le CASE
    q.addBindValue(cbStatut->currentText());
    q.addBindValue(spCout->value());
    q.addBindValue(teRapport->toPlainText());
    if (m_id) q.addBindValue(m_id);

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur Oracle", q.lastError().text());
        return;
    }
    QDialog::accept();
}
