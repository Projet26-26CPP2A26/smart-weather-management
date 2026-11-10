#include <QApplication>
#include <QMessageBox>

#include "database.h"
#include "mainwindow.h"
#include "style.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setStyleSheet(APP_STYLE);

    QString err;
    if (!Database::connect(&err)) {
        QMessageBox::critical(nullptr, "Connexion à la base",
            "Impossible d'ouvrir la base de données :\n\n" + err);
        return 1;
    }

    MainWindow w;
    w.show();
    return app.exec();
}
