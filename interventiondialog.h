#pragma once
#include <QDialog>
#include <QDate>

class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QTextEdit;

// Dialogue d'ajout / modification d'une intervention.
// id == 0  -> creation   |   id > 0 -> modification
class InterventionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InterventionDialog(int id = 0, QWidget *parent = nullptr);

    // Pre-remplissage (ex: depuis la planification predictive)
    void preset(int idStation, const QString &type, const QString &priorite, const QDate &datePlanifiee);

protected:
    void accept() override;

private:
    void loadCombos();
    void loadExisting();

    int m_id;
    QComboBox *cbStation, *cbEmploye, *cbType, *cbPriorite, *cbStatut;
    QDateEdit *dePlan;
    QDoubleSpinBox *spCout;
    QTextEdit *teRapport;
};
