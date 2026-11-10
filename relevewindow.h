#pragma once
#include <QMainWindow>
#include <QVariantList>

namespace Ui { class ReleveWindow; }
class QTableView;
class QSqlQuery;
class QSqlQueryModel;
class QLineEdit;
class QDateEdit;
class QCheckBox;
class QComboBox;

struct MonthStat { QString month; double mn, avg, mx, precip; };

class ReleveWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ReleveWindow(QWidget *parent = nullptr);
    ~ReleveWindow();
    void refreshData() { refresh(); }

private slots:
    void refresh();          // Lecture + recherche + tri
    void addReleve();        // C
    void editReleve();       // U
    void deleteReleve();     // D
    void exportPdf();        // Export PDF
    void showHistogram();    // Statistiques mensuelles
    void showAttestation();  // Métier innovant 2
    void showAnomalies();    // Métier innovant 1

private:
    Ui::ReleveWindow *ui;
    QTableView *view;
    QSqlQueryModel *model;
    QLineEdit *stationEdit;
    QDateEdit *fromEdit, *toEdit;
    QCheckBox *useDates;
    QComboBox *sortBy, *sortDir;

    int currentRow() const;
    QString whereClause(QVariantList &binds) const;
    QSqlQuery run(const QString &sql, const QVariantList &binds = {});
    QList<MonthStat> monthlyStats();
    void fail(const QSqlQuery &q);
    void showText(const QString &title, const QString &text);
};
