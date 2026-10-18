#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MaintenanceWindow; }
QT_END_NAMESPACE

class SlaModel;

class MaintenanceWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MaintenanceWindow(const QString &role, QWidget *parent = nullptr);
    ~MaintenanceWindow() override;

    // Point d'entree depuis le Module 3 : demande le role (ou reprend celui de l'application)
    // puis ouvre la fenetre Module 5 (une seule instance).
    static void open(QWidget *parent = nullptr);

private slots:
    void reload();
    void addIntervention();
    void editIntervention();
    void deleteIntervention();
    void archiveIntervention();
    void exportCsv();
    void exportHistoryPdf();
    void generateMissionOrder();
    void showSlaDelays();
    void predictivePlanning();
    void refreshStats();
    void changeRole();

private:
    int selectedId() const;
    void applyRole();
    bool allowed(const QString &action, const QString &title);

    QString m_role;

    Ui::MaintenanceWindow *ui;
    SlaModel *m_model;
};
