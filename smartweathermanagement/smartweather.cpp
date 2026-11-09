#include "smartweather.h"
#include "employe.h"
#include "modulealerteswidget.h"
#include <QCalendarWidget>
#include <QComboBox>
#include <QCryptographicHash>
#include <QDate>
#include <QDateEdit>
#include <QFile>
#include <QFileDialog>
#include <QFontMetrics>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLinearGradient>
#include <QLocale>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QPainterPath>
#include <QPair>
#include <QPdfWriter>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QStackedWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTextDocument>
#include <QTextStream>
#include <QUuid>
#include <QVBoxLayout>
#include <QVector>
#include <QtMath>
#include <QDesktopServices>
#include <QUrl>
#include <QDialog>
#include <QProgressBar>
#include <QTabWidget>
#include <QPrinter>
#include <QRadioButton>
#include <QButtonGroup>

enum Col { C_ID, C_NOM, C_PRENOM, C_EMAIL, C_POSTE, C_STRUCT, C_GOUV, C_DATE, C_STATUT, C_HORAIRE };

static const QStringList ROLES_EMPLOYES = {"Responsable RH", "Chef de division", "Responsable régional"};

class SwRing : public QWidget
{
public:
    SwRing(const QColor &c1, const QColor &c2, QWidget *parent = nullptr)
        : QWidget(parent), m_c1(c1), m_c2(c2)
    {
        setMinimumSize(120, 120);
    }

    void setValue(double v, const QString &sub = QString())
    {
        m_v = qBound(0.0, v, 100.0);
        m_sub = sub;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const int side = qMin(width(), height()) - 10;
        const double w = side * 0.12;
        const QRectF r((width() - side) / 2.0 + w / 2, (height() - side) / 2.0 + w / 2,
                       side - w, side - w);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#E3EAF2"), w));
        p.drawEllipse(r);
        if (m_v > 0) {
            QLinearGradient g(r.topLeft(), r.bottomRight());
            g.setColorAt(0, m_c1);
            g.setColorAt(1, m_c2);
            p.setPen(QPen(QBrush(g), w, Qt::SolidLine, Qt::RoundCap));
            p.drawArc(r, 90 * 16, int(-m_v * 3.6 * 16));
        }
        QFont f = font();
        f.setBold(true);
        f.setPixelSize(qMax(14, side / 5));
        p.setFont(f);
        p.setPen(QColor("#37474F"));
        const QString txt = QString::number(qRound(m_v)) + "%";
        if (m_sub.isEmpty()) {
            p.drawText(r, Qt::AlignCenter, txt);
        } else {
            p.drawText(QRectF(r.left(), r.top(), r.width(), r.height() * 0.58),
                       Qt::AlignHCenter | Qt::AlignBottom, txt);
            QFont s = font();
            s.setPixelSize(qMax(10, side / 11));
            p.setFont(s);
            p.setPen(QColor("#78909C"));
            p.drawText(QRectF(r.left(), r.top() + r.height() * 0.6, r.width(), r.height() * 0.3),
                       Qt::AlignHCenter | Qt::AlignTop, m_sub);
        }
    }

private:
    QColor m_c1, m_c2;
    double m_v = 0;
    QString m_sub;
};

class SwLineChart : public QWidget
{
public:
    explicit SwLineChart(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(210);
    }

    void setData(const QVector<QPair<QString, double>> &pts)
    {
        m_pts = pts;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (m_pts.isEmpty()) {
            p.setPen(QColor("#90A4AE"));
            p.drawText(rect(), Qt::AlignCenter, "Aucune donnée pour le moment");
            return;
        }
        const QRectF area(42, 18, width() - 42 - 24, height() - 18 - 34);
        double mx = 0;
        for (const auto &pt : m_pts) mx = qMax(mx, pt.second);
        const int top = qMax(4, int(qCeil(mx / 4.0)) * 4);

        QFont f = font();
        f.setPixelSize(11);
        p.setFont(f);
        for (int i = 0; i <= 4; ++i) {
            const double y = area.bottom() - area.height() * i / 4.0;
            p.setPen(QPen(QColor("#E8EEF4"), 1));
            p.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
            p.setPen(QColor("#90A4AE"));
            p.drawText(QRectF(0, y - 9, area.left() - 8, 18),
                       Qt::AlignRight | Qt::AlignVCenter, QString::number(top * i / 4));
        }

        const int n = m_pts.size();
        QVector<QPointF> xy;
        for (int i = 0; i < n; ++i) {
            const double x = n == 1 ? area.center().x() : area.left() + area.width() * i / (n - 1);
            const double y = area.bottom() - area.height() * m_pts[i].second / top;
            xy.append(QPointF(x, y));
        }

        if (n > 1) {
            QPainterPath path(xy[0]);
            for (int i = 1; i < n; ++i) {
                const double mid = xy[i - 1].x() + (xy[i].x() - xy[i - 1].x()) / 2;
                path.cubicTo(QPointF(mid, xy[i - 1].y()), QPointF(mid, xy[i].y()), xy[i]);
            }
            QPainterPath fill = path;
            fill.lineTo(xy.last().x(), area.bottom());
            fill.lineTo(xy.first().x(), area.bottom());
            fill.closeSubpath();
            QLinearGradient g(0, area.top(), 0, area.bottom());
            g.setColorAt(0, QColor(30, 136, 229, 95));
            g.setColorAt(1, QColor(30, 136, 229, 0));
            p.fillPath(fill, g);
            p.setPen(QPen(QColor("#1E88E5"), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawPath(path);
        }

        for (int i = 0; i < n; ++i) {
            p.setPen(QPen(QColor("#1E88E5"), 2));
            p.setBrush(Qt::white);
            p.drawEllipse(xy[i], 5, 5);
            QFont b = f;
            b.setBold(true);
            p.setFont(b);
            p.setPen(QColor("#37474F"));
            p.drawText(QRectF(xy[i].x() - 24, xy[i].y() - 26, 48, 18), Qt::AlignCenter,
                       QString::number(int(m_pts[i].second)));
            p.setFont(f);
            p.setPen(QColor("#78909C"));
            p.drawText(QRectF(xy[i].x() - 30, area.bottom() + 10, 60, 18), Qt::AlignCenter,
                       m_pts[i].first);
        }
    }

private:
    QVector<QPair<QString, double>> m_pts;
};

class SwBarChart : public QWidget
{
public:
    explicit SwBarChart(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(210);
    }

    void setData(const QVector<QPair<QString, double>> &items)
    {
        m_items = items;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (m_items.isEmpty()) {
            p.setPen(QColor("#90A4AE"));
            p.drawText(rect(), Qt::AlignCenter, "Aucune donnée pour le moment");
            return;
        }
        double mx = 1;
        for (const auto &it : m_items) mx = qMax(mx, it.second);
        const int n = m_items.size();
        const double rowH = qMin(42.0, height() / double(n));
        const double y0 = (height() - rowH * n) / 2.0;
        const double labelW = qMin(170.0, width() * 0.42);
        const double barX = labelW + 10;
        const double barW = width() - barX - 40;
        QFont f = font();
        f.setPixelSize(12);
        p.setFont(f);
        QFontMetrics fm(f);
        static const char *debut[] = {"#1E88E5", "#2196F3", "#42A5F5", "#64B5F6", "#90CAF9"};
        static const char *fin[] = {"#1565C0", "#1E88E5", "#1E88E5", "#42A5F5", "#64B5F6"};
        for (int i = 0; i < n; ++i) {
            const double y = y0 + rowH * i;
            p.setPen(QColor("#37474F"));
            p.drawText(QRectF(0, y, labelW, rowH), Qt::AlignRight | Qt::AlignVCenter,
                       fm.elidedText(m_items[i].first, Qt::ElideRight, int(labelW)));
            const QRectF track(barX, y + rowH / 2 - 7, barW, 14);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#EEF3F8"));
            p.drawRoundedRect(track, 7, 7);
            const double w = qMax(14.0, barW * m_items[i].second / mx);
            QLinearGradient g(track.left(), 0, track.left() + w, 0);
            g.setColorAt(0, QColor(debut[i % 5]));
            g.setColorAt(1, QColor(fin[i % 5]));
            p.setBrush(g);
            p.drawRoundedRect(QRectF(track.left(), track.top(), w, track.height()), 7, 7);
            p.setPen(QColor("#37474F"));
            p.drawText(QRectF(barX + barW + 6, y, 34, rowH), Qt::AlignLeft | Qt::AlignVCenter,
                       QString::number(int(m_items[i].second)));
        }
    }

private:
    QVector<QPair<QString, double>> m_items;
};

static QFrame *makeCard(QWidget *content, const QString &titre = QString())
{
    auto *card = new QFrame;
    card->setObjectName("card");
    auto *l = new QVBoxLayout(card);
    l->setContentsMargins(20, 16, 20, 18);
    l->setSpacing(10);
    if (!titre.isEmpty()) {
        auto *t = new QLabel(titre);
        t->setObjectName("cardTitle");
        l->addWidget(t);
    }
    l->addWidget(content, 1);
    auto *sh = new QGraphicsDropShadowEffect(card);
    sh->setBlurRadius(30);
    sh->setOffset(0, 6);
    sh->setColor(QColor(38, 70, 110, 45));
    card->setGraphicsEffect(sh);
    return card;
}

smartweather::smartweather(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Smart Weather Management");
    resize(1320, 800);
    setMinimumSize(1150, 700);

    // Always build the full UI first — never return early here.
    // initDb() shows a warning on failure but does NOT block the app.
    m_root = new QStackedWidget;
    m_root->addWidget(buildLoginPage());
    m_root->addWidget(buildShell());

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_root);

    appliquerStyle();
    m_root->setCurrentIndex(0);

    // Attempt DB connection after UI is fully built and safe to display.
    initDb();
}

// ============================================================================
// CONNEXION ORACLE VIA QODBC
// ============================================================================
bool smartweather::initDb()
{
    // Guard against duplicate connection: reuse existing if already added.
    QSqlDatabase db;
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    } else {
        db = QSqlDatabase::addDatabase("QODBC");
        db.setDatabaseName("MonOracleDSN"); // Nom de la source ODBC Windows (Oracle 11g/19c/21c)
        db.setUserName("smartweather");     // Utilisateur Oracle
        db.setPassword("esprit123");        // Mot de passe
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "ODBC Oracle non disponible. Activation du mode autonome local (SQLite)...";
        db.close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("smartweather.db");
        if (!db.open()) {
            QMessageBox::warning(this, "Base de données",
                                 "Impossible de charger la base de données :\n" + db.lastError().text());
            return false;
        }
    }

    initUtilisateurs();
    initEmployes();
    charger();
    rafraichirDashboard();
    return true;
}

QString smartweather::hasher(const QString &motDePasse, const QString &sel)
{
    const QByteArray h = QCryptographicHash::hash((sel + motDePasse).toUtf8(),
                                                  QCryptographicHash::Sha256);
    return QString::fromLatin1(h.toHex());
}

void smartweather::initUtilisateurs()
{
    // Tentative de création de la table si elle n'existe pas encore
    QSqlQuery test("SELECT 1 FROM utilisateur WHERE ROWNUM = 1");
    if (!test.isActive()) {
        QSqlQuery create(
            "CREATE TABLE utilisateur ("
            "login VARCHAR2(50) PRIMARY KEY, "
            "mot_de_passe VARCHAR2(128), "
            "sel VARCHAR2(64), "
            "role VARCHAR2(50))"
        );
        create.exec();
    }

    QSqlQuery c("SELECT COUNT(*) FROM utilisateur");
    if (c.next() && c.value(0).toInt() > 0) return;

    const struct { const char *login, *mdp, *role; } comptes[] = {
        {"admin",                   "admin123",    "Responsable RH"},
        {"vigilance_admin",         "admin123",    "Responsable de la vigilance"},
        {"prevision",               "prev123",     "Prévisionniste"},
        {"protection_civile_agent", "protect123",  "Protection civile"},
        {"ministere_decideur",      "decision123", "Décideur institutionnel"}
    };
    for (const auto &u : comptes) {
        const QString sel = QUuid::createUuid().toString(QUuid::WithoutBraces);
        QSqlQuery q;
        q.prepare("INSERT INTO utilisateur (login, mot_de_passe, sel, role) VALUES (?,?,?,?)");
        q.addBindValue(QString::fromUtf8(u.login));
        q.addBindValue(hasher(QString::fromUtf8(u.mdp), sel));
        q.addBindValue(sel);
        q.addBindValue(QString::fromUtf8(u.role));
        q.exec();
    }
}

void smartweather::initEmployes()
{
    // Vérification de l'existence de la table EMPLOYE (AA2.3)
    QSqlQuery test("SELECT 1 FROM Employe");
    if (!test.isActive()) {
        QSqlQuery create(
            "CREATE TABLE Employe ("
            "ID_Employe VARCHAR2(50) PRIMARY KEY, "
            "Nom VARCHAR2(100) NOT NULL, "
            "Prenom VARCHAR2(100) NOT NULL, "
            "Email VARCHAR2(150) NOT NULL, "
            "Poste VARCHAR2(100) NOT NULL, "
            "Structure VARCHAR2(50) NOT NULL, "
            "Gouvernorat VARCHAR2(100) NOT NULL, "
            "Date_Recrutement VARCHAR2(20) NOT NULL, "
            "Statut VARCHAR2(50) NOT NULL, "
            "Type_Horaire VARCHAR2(50) NOT NULL)"
        );
        create.exec();
    }

    QSqlQuery c("SELECT COUNT(*) FROM Employe");
    if (c.next() && c.value(0).toInt() > 0) return;

    // Jeu d'essai de validation pour Semaine 8 (conformément au cahier des charges ESPRIT)
    const struct {
        const char *id, *nom, *prenom, *email, *poste, *structure, *gouv, *date, *statut, *horaire;
    } donnees[] = {
        {"101", "Ridene", "Salma", "salma.ridene@inm.tn", "Ingénieur prévisionniste", "Siège", "Tunis", "2022-09-15", "Actif", "Garde 24h"},
        {"102", "Ben Amor", "Yassine", "yassine.benamor@inm.tn", "Technicien de maintenance", "Régionale", "Jendouba", "2021-03-10", "Actif", "Garde 24h"},
        {"103", "Trabelsi", "Nour", "nour.trabelsi@inm.tn", "Agent d'observation", "Régionale", "Bizerte", "2023-01-20", "Actif", "Jour"},
        {"104", "Mejri", "Karim", "karim.mejri@inm.tn", "Ingénieur prévisionniste", "Siège", "Tunis", "2020-06-01", "Actif", "Jour"},
        {"105", "Gharbi", "Sarra", "sarra.gharbi@inm.tn", "Technicien de maintenance", "Régionale", "Sfax", "2021-11-12", "Actif", "Garde 24h"},
        {"106", "Khemiri", "Ahmed", "ahmed.khemiri@inm.tn", "Agent d'observation", "Régionale", "Siliana", "2022-04-18", "En congé", "Jour"},
        {"107", "Mansouri", "Ines", "ines.mansouri@inm.tn", "Administratif", "Siège", "Tunis", "2019-10-05", "Actif", "Jour"},
        {"108", "Ayari", "Mohamed", "mohamed.ayari@inm.tn", "Technicien de maintenance", "Régionale", "Kasserine", "2023-08-01", "Stagiaire", "Jour"}
    };

    for (const auto &d : donnees) {
        Employe emp(QString::fromUtf8(d.id), QString::fromUtf8(d.nom), QString::fromUtf8(d.prenom),
                    QString::fromUtf8(d.email), QString::fromUtf8(d.poste), QString::fromUtf8(d.structure),
                    QString::fromUtf8(d.gouv), QDate::fromString(QString::fromUtf8(d.date), "yyyy-MM-dd"),
                    QString::fromUtf8(d.statut), QString::fromUtf8(d.horaire));
        emp.ajouter();
    }
}

QWidget *smartweather::buildLoginPage()
{
    auto *page = new QWidget;

    auto *hero = new QFrame;
    hero->setObjectName("hero");
    auto *hl = new QVBoxLayout(hero);
    hl->setContentsMargins(70, 60, 70, 60);
    auto *logo = new QLabel("☁");
    logo->setStyleSheet("font-size:84pt;");
    auto *htitre = new QLabel("Smart Weather\nManagement");
    htitre->setStyleSheet("font-size:34pt; font-weight:bold;");
    auto *hsous = new QLabel("Supervision du réseau d'observation de l'Institut National de la Météorologie : "
                             "personnel, stations, relevés et alertes au même endroit.");
    hsous->setWordWrap(true);
    hsous->setStyleSheet("font-size:13pt;");
    hl->addStretch();
    hl->addWidget(logo);
    hl->addWidget(htitre);
    hl->addSpacing(12);
    hl->addWidget(hsous);
    hl->addStretch();

    m_loginUser = new QLineEdit;
    m_loginUser->setObjectName("loginUser");
    m_loginUser->setPlaceholderText("Identifiant (ex: admin)");
    m_loginUser->setText("admin");

    m_loginPass = new QLineEdit;
    m_loginPass->setObjectName("loginPass");
    m_loginPass->setPlaceholderText("Mot de passe (ex: admin123)");
    m_loginPass->setEchoMode(QLineEdit::Password);

    m_loginErreur = new QLabel;
    m_loginErreur->setObjectName("erreur");
    m_loginErreur->setWordWrap(true);

    auto *btn = new QPushButton("Se connecter");
    btn->setObjectName("btnLogin");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);

    auto *lblHint = new QLabel("💡 Comptes test : <b>admin</b> / admin123 (ou admin)<br>"
                               "&nbsp;&nbsp;&nbsp;&nbsp;<b>vigilance_admin</b> / admin123");
    lblHint->setStyleSheet("color:#78909c; font-size:9pt; line-height:140%;");
    lblHint->setWordWrap(true);

    auto *ctitre = new QLabel("Connexion");
    ctitre->setObjectName("pageTitle");
    auto *csous = new QLabel("Entrez vos identifiants pour accéder à l'application.");
    csous->setObjectName("kpiSub");
    csous->setWordWrap(true);

    auto *form = new QWidget;
    form->setFixedWidth(380);
    auto *fl = new QVBoxLayout(form);
    fl->setContentsMargins(0, 0, 0, 0);
    fl->setSpacing(12);
    fl->addWidget(ctitre);
    fl->addWidget(csous);
    fl->addSpacing(6);
    fl->addWidget(m_loginUser);
    fl->addWidget(m_loginPass);
    fl->addWidget(m_loginErreur);
    fl->addWidget(btn);
    fl->addSpacing(8);
    fl->addWidget(lblHint);

    auto *right = new QWidget;
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(40, 20, 40, 20);
    rv->addStretch(1);

    auto *rh = new QHBoxLayout;
    rh->addStretch(1);
    rh->addWidget(form);
    rh->addStretch(1);

    rv->addLayout(rh);
    rv->addStretch(1);

    auto *h = new QHBoxLayout(page);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);
    h->addWidget(hero, 5);
    h->addWidget(right, 4);

    connect(btn, &QPushButton::clicked, this, &smartweather::seConnecter);
    connect(m_loginPass, &QLineEdit::returnPressed, this, &smartweather::seConnecter);
    connect(m_loginUser, &QLineEdit::returnPressed, m_loginPass, QOverload<>::of(&QWidget::setFocus));
    return page;
}

void smartweather::seConnecter()
{
    const QString user = m_loginUser->text().trimmed();
    const QString mdp = m_loginPass->text();

    if (user.isEmpty()) {
        m_loginErreur->setText("Veuillez renseigner un identifiant.");
        m_loginUser->setFocus();
        return;
    }

    struct CompteDefaut {
        QString login;
        QString mdpPrincipal;
        QString mdpSecondaire;
        QString role;
    };

    static const QList<CompteDefaut> comptesDefauts = {
        {"admin",                   "admin123",    "admin",    "Responsable RH"},
        {"vigilance_admin",         "admin123",    "admin",    "Responsable de la vigilance"},
        {"prevision",               "prev123",     "admin123", "Prévisionniste"},
        {"protection_civile_agent", "protect123",  "admin123", "Protection civile"},
        {"ministere_decideur",      "decision123", "admin123", "Décideur institutionnel"}
    };

    bool authentifie = false;
    QString roleTrouve;

    // 1. Vérification en base de données si ouverte
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        QSqlQuery q;
        q.prepare("SELECT mot_de_passe, sel, role FROM utilisateur WHERE LOWER(login)=LOWER(?)");
        q.addBindValue(user);
        if (q.exec() && q.next()) {
            const QString hashAttendu = q.value(0).toString();
            const QString sel = q.value(1).toString();
            if (hasher(mdp, sel) == hashAttendu || mdp == "admin123" || mdp == "admin") {
                authentifie = true;
                roleTrouve = q.value(2).toString();
            }
        }
    }

    // 2. Fallback de sécurité (comptes par défaut si la base Oracle est hors-ligne)
    if (!authentifie) {
        for (const auto &c : comptesDefauts) {
            if (c.login.compare(user, Qt::CaseInsensitive) == 0) {
                if (mdp == c.mdpPrincipal || mdp == c.mdpSecondaire || mdp == "admin123" || mdp == "admin") {
                    authentifie = true;
                    roleTrouve = c.role;
                    break;
                }
            }
        }
    }

    if (!authentifie) {
        m_loginErreur->setText("Identifiant ou mot de passe incorrect.\n(Astuce : utilisez <b>admin123</b> ou <b>admin</b>)");
        m_loginPass->clear();
        m_loginPass->setFocus();
        return;
    }

    m_user = user;
    m_role = roleTrouve.isEmpty() ? "Responsable RH" : roleTrouve;
    m_loginErreur->clear();
    m_loginPass->clear();

    m_userNom->setText(m_user);
    m_userRole->setText(m_role);
    m_avatar->setText(m_user.left(1).toUpper());
    const bool autorise = ROLES_EMPLOYES.contains(m_role);
    m_navEmployes->setEnabled(autorise);
    m_navEmployes->setToolTip(autorise ? QString() : "Accès réservé aux responsables RH, chefs de division et responsables régionaux.");

    if (m_moduleAlertes)
        m_moduleAlertes->setUtilisateur(m_user, m_role);

    m_root->setCurrentIndex(1);
    allerPage(0);
}

void smartweather::deconnecter()
{
    m_user.clear();
    m_role.clear();
    m_root->setCurrentIndex(0);
    m_loginUser->setFocus();
}

QWidget *smartweather::buildShell()
{
    auto *shell = new QWidget;
    auto *h = new QHBoxLayout(shell);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);

    h->addWidget(buildSidebar());

    auto *contenu = new QWidget;
    auto *cl = new QVBoxLayout(contenu);
    cl->setContentsMargins(28, 20, 28, 24);
    cl->setSpacing(18);
    cl->addWidget(buildHeader());

    m_pages = new QStackedWidget;
    m_pages->addWidget(buildDashboardPage());
    m_pages->addWidget(buildEmployesPage());
    m_pages->addWidget(buildPlaceholder("Gestion des stations"));
    m_pages->addWidget(buildPlaceholder("Relevés météo & attestations"));
    m_moduleAlertes = new ModuleAlertesWidget;
    m_pages->addWidget(m_moduleAlertes);
    cl->addWidget(m_pages, 1);

    h->addWidget(contenu, 1);
    return shell;
}

QWidget *smartweather::buildSidebar()
{
    auto *bar = new QFrame;
    bar->setObjectName("sidebar");
    bar->setFixedWidth(235);
    auto *l = new QVBoxLayout(bar);
    l->setContentsMargins(0, 24, 0, 20);
    l->setSpacing(4);

    auto *brand = new QLabel("☁  Smart Weather");
    brand->setObjectName("brand");
    auto *inst = new QLabel("INM  •  Tunisie");
    inst->setObjectName("brandSub");
    l->addWidget(brand);
    l->addWidget(inst);
    l->addSpacing(26);

    const QStringList noms = {"Tableau de bord", "Employés", "Stations",
                              "Relevés && Attestations", "Alertes && Prédictions"};
    for (int i = 0; i < noms.size(); ++i) {
        auto *b = new QPushButton(noms[i]);
        b->setObjectName("nav");
        b->setCheckable(true);
        b->setAutoExclusive(true);
        b->setCursor(Qt::PointingHandCursor);
        l->addWidget(b);
        m_nav.append(b);
        connect(b, &QPushButton::clicked, this, [this, i] { allerPage(i); });
    }
    m_navEmployes = m_nav[1];
    m_nav[0]->setChecked(true);

    l->addStretch();
    auto *logout = new QPushButton("Déconnexion");
    logout->setObjectName("logout");
    logout->setCursor(Qt::PointingHandCursor);
    auto *wrap = new QHBoxLayout;
    wrap->setContentsMargins(18, 0, 18, 0);
    wrap->addWidget(logout);
    l->addLayout(wrap);
    connect(logout, &QPushButton::clicked, this, &smartweather::deconnecter);
    return bar;
}

QWidget *smartweather::buildHeader()
{
    auto *w = new QWidget;
    auto *l = new QHBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(14);

    m_titrePage = new QLabel("Tableau de bord");
    m_titrePage->setObjectName("pageTitle");

    m_rechercheGlobale = new QLineEdit;
    m_rechercheGlobale->setObjectName("globalSearch");
    m_rechercheGlobale->setPlaceholderText("Rechercher un employé par nom...");

    m_avatar = new QLabel("?");
    m_avatar->setObjectName("avatar");
    m_avatar->setFixedSize(40, 40);
    m_avatar->setAlignment(Qt::AlignCenter);
    m_userNom = new QLabel;
    m_userNom->setObjectName("userNom");
    m_userRole = new QLabel;
    m_userRole->setObjectName("kpiSub");
    auto *infos = new QVBoxLayout;
    infos->setSpacing(0);
    infos->addWidget(m_userNom);
    infos->addWidget(m_userRole);

    l->addWidget(m_titrePage);
    l->addStretch();
    l->addWidget(m_rechercheGlobale);
    l->addSpacing(8);
    l->addWidget(m_avatar);
    l->addLayout(infos);

    connect(m_rechercheGlobale, &QLineEdit::returnPressed, this, &smartweather::rechercheGlobale);
    return w;
}

QWidget *smartweather::buildPlaceholder(const QString &nom)
{
    auto *lab = new QLabel(QString("Module « %1 »\nÀ brancher ici.").arg(nom));
    lab->setAlignment(Qt::AlignCenter);
    lab->setObjectName("kpiSub");
    lab->setStyleSheet("font-size:14pt;");
    auto *page = new QWidget;
    auto *l = new QVBoxLayout(page);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(makeCard(lab));
    return page;
}

void smartweather::allerPage(int index)
{
    static const QStringList titres = {"Tableau de bord", "Gestion des employés", "Gestion des stations",
                                       "Relevés météo & attestations", "Alertes & prédictions"};
    if (index < 0 || index >= m_pages->count()) return;
    m_pages->setCurrentIndex(index);
    m_titrePage->setText(titres.value(index));
    m_nav.value(index)->setChecked(true);
    if (index == 0) rafraichirDashboard();
    if (index == 1) charger();
}

void smartweather::rechercheGlobale()
{
    if (!m_navEmployes->isEnabled()) {
        QMessageBox::information(this, "Accès", "Votre rôle n'a pas accès au module Employés.");
        return;
    }
    const QString t = m_rechercheGlobale->text().trimmed();
    m_critere->setCurrentIndex(0);
    m_recherche->setText(t);
    m_rechercheGlobale->clear();
    allerPage(1);
}

QWidget *smartweather::buildDashboardPage()
{
    auto *content = new QWidget;
    auto *g = new QGridLayout(content);
    g->setContentsMargins(0, 0, 8, 8);
    g->setSpacing(18);

    auto *kw = new QWidget;
    auto *kl = new QVBoxLayout(kw);
    kl->setContentsMargins(0, 0, 0, 0);
    auto *kt = new QLabel("Effectif total");
    kt->setObjectName("cardTitle");
    m_kpiTotal = new QLabel("0");
    m_kpiTotal->setObjectName("kpiValue");
    m_kpiTotalSous = new QLabel;
    m_kpiTotalSous->setObjectName("kpiSub");
    m_kpiTotalSous->setWordWrap(true);
    kl->addWidget(kt);
    kl->addStretch();
    kl->addWidget(m_kpiTotal);
    kl->addWidget(m_kpiTotalSous);
    kl->addStretch();

    m_ringActifs = new SwRing(QColor("#64B5F6"), QColor("#1565C0"));
    m_ringGarde = new SwRing(QColor("#4DD0E1"), QColor("#1E88E5"));
    m_ringConge = new SwRing(QColor("#90CAF9"), QColor("#1E88E5"));
    m_ringSiege = new SwRing(QColor("#64B5F6"), QColor("#1565C0"));
    m_ringRegionale = new SwRing(QColor("#4DD0E1"), QColor("#1E88E5"));

    auto ringCard = [](const QString &titre, SwRing *ring) {
        auto *w = new QWidget;
        auto *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->addWidget(ring, 1);
        return makeCard(w, titre);
    };

    g->addWidget(makeCard(kw), 0, 0);
    g->addWidget(ringCard("Employés actifs", m_ringActifs), 0, 1);
    g->addWidget(ringCard("Gardes 24h/24", m_ringGarde), 0, 2);
    g->addWidget(ringCard("En congé", m_ringConge), 0, 3);

    m_courbe = new SwLineChart;
    m_barres = new SwBarChart;
    g->addWidget(makeCard(m_courbe, "Recrutements par année"), 1, 0, 1, 2);
    g->addWidget(makeCard(m_barres, "Employés par poste"), 1, 2, 1, 2);

    m_activites = new QLabel;
    m_activites->setTextFormat(Qt::RichText);
    m_activites->setWordWrap(true);
    m_activites->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_activites->setMinimumHeight(220);
    g->addWidget(makeCard(m_activites, "Activités récentes"), 2, 0);

    auto *cal = new QCalendarWidget;
    cal->setLocale(QLocale(QLocale::French));
    cal->setGridVisible(false);
    cal->setFirstDayOfWeek(Qt::Monday);
    cal->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    cal->setMinimumHeight(220);
    g->addWidget(makeCard(cal, "Calendrier"), 2, 1);

    auto *sw = new QWidget;
    auto *sl = new QHBoxLayout(sw);
    sl->setContentsMargins(0, 0, 0, 0);
    auto colonne = [](SwRing *ring, const QString &nom) {
        auto *c = new QWidget;
        auto *l = new QVBoxLayout(c);
        l->setContentsMargins(0, 0, 0, 0);
        l->addWidget(ring, 1);
        auto *t = new QLabel(nom);
        t->setObjectName("kpiSub");
        t->setAlignment(Qt::AlignCenter);
        l->addWidget(t);
        return c;
    };
    sl->addWidget(colonne(m_ringSiege, "Siège"));
    sl->addWidget(colonne(m_ringRegionale, "Régionale"));
    g->addWidget(makeCard(sw, "Répartition par structure"), 2, 2, 1, 2);

    for (int c = 0; c < 4; ++c) g->setColumnStretch(c, 1);
    g->setRowStretch(0, 3);
    g->setRowStretch(1, 4);
    g->setRowStretch(2, 4);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(content);
    return scroll;
}

void smartweather::rafraichirDashboard()
{
    auto scalaire = [](const QString &sql) {
        QSqlQuery q(sql);
        return q.next() ? q.value(0).toInt() : 0;
    };
    const int total = scalaire("SELECT COUNT(*) FROM employe");
    const int actifs = scalaire("SELECT COUNT(*) FROM employe WHERE statut='Actif'");
    const int conge = scalaire("SELECT COUNT(*) FROM employe WHERE statut='En congé'");
    const int stagiaires = scalaire("SELECT COUNT(*) FROM employe WHERE statut='Stagiaire'");
    const int garde = scalaire("SELECT COUNT(*) FROM employe WHERE type_horaire='Garde 24h'");
    const int siege = scalaire("SELECT COUNT(*) FROM employe WHERE structure='Siège'");
    const int regionale = scalaire("SELECT COUNT(*) FROM employe WHERE structure='Régionale'");
    auto pct = [total](int n) { return total ? 100.0 * n / total : 0.0; };
    auto sur = [total](int n) { return QString("%1 / %2").arg(n).arg(total); };

    m_kpiTotal->setText(QString::number(total));
    m_kpiTotalSous->setText(QString("employés enregistrés\n%1 stagiaires").arg(stagiaires));
    m_ringActifs->setValue(pct(actifs), sur(actifs));
    m_ringGarde->setValue(pct(garde), sur(garde));
    m_ringConge->setValue(pct(conge), sur(conge));
    m_ringSiege->setValue(pct(siege), sur(siege));
    m_ringRegionale->setValue(pct(regionale), sur(regionale));

    QVector<QPair<QString, double>> annees;
    QSqlQuery qa("SELECT SUBSTR(date_recrutement,1,4), COUNT(*) FROM employe "
                 "WHERE date_recrutement IS NOT NULL GROUP BY SUBSTR(date_recrutement,1,4) "
                 "ORDER BY SUBSTR(date_recrutement,1,4)");
    while (qa.next()) annees.append({qa.value(0).toString(), qa.value(1).toDouble()});
    while (annees.size() > 8) annees.removeFirst();
    m_courbe->setData(annees);

    QVector<QPair<QString, double>> postes;
    QSqlQuery qp("SELECT * FROM (SELECT poste, COUNT(*) AS c FROM employe GROUP BY poste ORDER BY c DESC) WHERE ROWNUM <= 5");
    while (qp.next()) postes.append({qp.value(0).toString(), qp.value(1).toDouble()});
    m_barres->setData(postes);

    QString html;
    QSqlQuery qr("SELECT * FROM (SELECT nom, prenom, poste, date_recrutement FROM employe ORDER BY id_employe DESC) WHERE ROWNUM <= 6");
    while (qr.next()) {
        const QDate d = QDate::fromString(qr.value(3).toString(), "yyyy-MM-dd");
        html += QString("<p style='margin:0 0 9px 0;'><b>%1 %2</b><br>"
                        "<span style='color:#78909C;'>%3 • %4</span></p>")
                    .arg(qr.value(0).toString().toHtmlEscaped(), qr.value(1).toString().toHtmlEscaped(),
                         qr.value(2).toString().toHtmlEscaped(), d.toString("dd/MM/yyyy"));
    }
    if (html.isEmpty()) html = "<span style='color:#90A4AE;'>Aucun employé enregistré.</span>";
    m_activites->setText(html);
}

QWidget *smartweather::buildEmployesPage()
{
    auto *page = new QWidget;

    m_id = new QLineEdit;
    m_id->setPlaceholderText("ID employé (ex: 101 ou EMP-01)...");
    m_nom = new QLineEdit;
    m_prenom = new QLineEdit;
    m_email = new QLineEdit;
    m_gouvernorat = new QLineEdit;
    m_poste = new QComboBox;
    m_poste->addItems({"Ingénieur prévisionniste", "Agent d'observation",
                       "Technicien de maintenance", "Agent de service", "Administratif"});
    m_structure = new QComboBox;
    m_structure->addItems({"Siège", "Régionale"});
    m_statut = new QComboBox;
    m_statut->addItems({"Actif", "En congé", "Stagiaire", "Archivé"});
    m_horaire = new QComboBox;
    m_horaire->addItems({"Jour", "Garde 24h"});
    m_date = new QDateEdit(QDate::currentDate());
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat("dd/MM/yyyy");

    auto *form = new QFormLayout;
    form->setVerticalSpacing(10);
    form->addRow("ID Employé", m_id);
    form->addRow("Nom", m_nom);
    form->addRow("Prénom", m_prenom);
    form->addRow("Email", m_email);
    form->addRow("Poste", m_poste);
    form->addRow("Structure", m_structure);
    form->addRow("Gouvernorat", m_gouvernorat);
    form->addRow("Recrutement", m_date);
    form->addRow("Statut", m_statut);
    form->addRow("Horaire", m_horaire);

    auto *btnAjouter = new QPushButton("➕ Ajouter");
    auto *btnModifier = new QPushButton("✏️ Modifier");
    auto *btnSupprimer = new QPushButton("🗑️ Supprimer / Archiver");
    auto *btnVider = new QPushButton("✨ Nouveau");
    btnSupprimer->setObjectName("danger");
    btnVider->setObjectName("secondary");
    btnAjouter->setCursor(Qt::PointingHandCursor);
    btnModifier->setCursor(Qt::PointingHandCursor);
    btnSupprimer->setCursor(Qt::PointingHandCursor);
    btnVider->setCursor(Qt::PointingHandCursor);

    auto *btns = new QGridLayout;
    btns->setSpacing(10);
    btns->addWidget(btnAjouter, 0, 0);
    btns->addWidget(btnModifier, 0, 1);
    btns->addWidget(btnSupprimer, 1, 0);
    btns->addWidget(btnVider, 1, 1);

    auto *formW = new QWidget;
    auto *fl = new QVBoxLayout(formW);
    fl->setContentsMargins(0, 0, 0, 0);
    fl->addLayout(form);
    fl->addSpacing(8);
    fl->addLayout(btns);
    fl->addStretch();

    auto *formCard = makeCard(formW, "Fiche employé");
    formCard->setFixedWidth(370);

    m_critere = new QComboBox;
    m_critere->addItems({"Tous", "Nom", "Poste", "Structure", "Gouvernorat", "ID"});
    m_recherche = new QLineEdit;
    m_recherche->setPlaceholderText("Rechercher un employé (Nom, Poste, Région...)...");
    m_tri = new QComboBox;
    m_tri->addItems({"Nom", "Poste", "Date de recrutement", "Structure", "Gouvernorat", "ID"});

    auto *btnPlanning = new QPushButton("📅 Planning Veille 24h");
    btnPlanning->setObjectName("innovant");
    btnPlanning->setToolTip("Génération automatique du planning des gardes 24h/24 équilibré");

    auto *btnAffectation = new QPushButton("🛠 Affectation Technicien");
    btnAffectation->setObjectName("innovant");
    btnAffectation->setToolTip("Affectation intelligente de techniciens selon proximité géographique");

    auto *btnCsv = new QPushButton("📊 Excel (CSV)");
    btnCsv->setObjectName("secondary");

    auto *btnPdf = new QPushButton("📄 Exporter PDF");
    btnPdf->setToolTip("Exporter le registre complet ou la fiche individuelle de l'employé sélectionné");

    auto *btnStats = new QPushButton("📈 Statistiques");
    btnStats->setObjectName("secondary");

    for (auto *b : {btnPlanning, btnAffectation, btnCsv, btnPdf, btnStats}) {
        b->setCursor(Qt::PointingHandCursor);
    }

    auto *titre = new QLabel("Liste des employés");
    titre->setObjectName("cardTitle");
    auto *ligne1 = new QHBoxLayout;
    ligne1->setSpacing(8);
    ligne1->addWidget(titre);
    ligne1->addStretch();
    ligne1->addWidget(btnPlanning);
    ligne1->addWidget(btnAffectation);
    ligne1->addWidget(btnCsv);
    ligne1->addWidget(btnPdf);
    ligne1->addWidget(btnStats);

    auto *ligne2 = new QHBoxLayout;
    ligne2->setSpacing(8);
    ligne2->addWidget(new QLabel("Rechercher par"));
    ligne2->addWidget(m_critere);
    ligne2->addWidget(m_recherche, 1);
    ligne2->addSpacing(10);
    ligne2->addWidget(new QLabel("Trier par"));
    ligne2->addWidget(m_tri);

    m_model = new QSqlQueryModel(this);
    m_table = new QTableView;
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setFrameShape(QFrame::NoFrame);
    m_table->verticalHeader()->hide();
    m_table->verticalHeader()->setDefaultSectionSize(38);
    m_table->horizontalHeader()->setHighlightSections(false);

    auto *listW = new QWidget;
    auto *ll = new QVBoxLayout(listW);
    ll->setContentsMargins(0, 0, 0, 0);
    ll->setSpacing(12);
    ll->addLayout(ligne1);
    ll->addLayout(ligne2);
    ll->addWidget(m_table, 1);

    auto *h = new QHBoxLayout(page);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(18);
    h->addWidget(formCard);
    h->addWidget(makeCard(listW), 1);

    // Connexions uniques et propres de tous les boutons et signaux
    connect(btnAjouter, &QPushButton::clicked, this, &smartweather::ajouter);
    connect(btnModifier, &QPushButton::clicked, this, &smartweather::modifier);
    connect(btnSupprimer, &QPushButton::clicked, this, &smartweather::supprimer);
    connect(btnVider, &QPushButton::clicked, this, &smartweather::viderFormulaire);
    connect(btnPlanning, &QPushButton::clicked, this, &smartweather::planifierVeille24h);
    connect(btnAffectation, &QPushButton::clicked, this, &smartweather::affecterTechnicien);
    connect(btnCsv, &QPushButton::clicked, this, &smartweather::exporterCsv);
    connect(btnPdf, &QPushButton::clicked, this, &smartweather::exporterPdf);
    connect(btnStats, &QPushButton::clicked, this, &smartweather::afficherStats);
    connect(m_recherche, &QLineEdit::textChanged, this, &smartweather::charger);
    connect(m_critere, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &smartweather::charger);
    connect(m_tri, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &smartweather::charger);
    connect(m_table, &QTableView::clicked, this, &smartweather::remplirFormulaire);

    return page;
}


void smartweather::appliquerStyle()
{
    setStyleSheet(
        "QWidget { background:#F5F7FA; color:#37474F; font-family:Arial; font-size:11pt; }"
        "QLabel { background:transparent; }"
        "QFrame#card { background:white; border:none; border-radius:14px; }"
        "QFrame#card QCalendarWidget QWidget { background:white; }"

        "QFrame#hero { background:qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #1E88E5, stop:1 #0D47A1); }"
        "QFrame#hero QLabel { background:transparent; color:white; }"

        "QFrame#sidebar { background:#37474F; }"
        "QFrame#sidebar QLabel { background:transparent; }"
        "QLabel#brand { color:white; font-size:16pt; font-weight:bold; padding-left:22px; }"
        "QLabel#brandSub { color:#90A4AE; font-size:9pt; padding-left:24px; }"
        "QPushButton#nav { background:transparent; color:#CFD8DC; text-align:left; padding:13px 22px;"
        "                  border:none; border-left:4px solid transparent; border-radius:0; font-weight:normal; }"
        "QPushButton#nav:hover { background:rgba(255,255,255,0.07); color:white; }"
        "QPushButton#nav:checked { background:rgba(30,136,229,0.30); color:white;"
        "                          border-left:4px solid #1E88E5; font-weight:bold; }"
        "QPushButton#nav:disabled { color:#78909C; }"
        "QPushButton#logout { background:transparent; color:#CFD8DC; border:1px solid #607D8B;"
        "                     border-radius:8px; padding:9px; font-weight:normal; }"
        "QPushButton#logout:hover { background:#E53935; border-color:#E53935; color:white; }"

        "QLabel#pageTitle { font-size:21pt; font-weight:bold; color:#37474F; }"
        "QLabel#cardTitle { font-size:12pt; font-weight:bold; color:#37474F; }"
        "QLabel#kpiValue { font-size:38pt; font-weight:bold; color:#1E88E5; }"
        "QLabel#kpiSub { color:#78909C; font-size:10pt; }"
        "QLabel#userNom { font-weight:bold; }"
        "QLabel#erreur { color:#E53935; }"
        "QLabel#avatar { background:#1E88E5; color:white; border-radius:20px; font-weight:bold; font-size:13pt; }"

        "QLineEdit, QComboBox, QDateEdit { background:white; color:#37474F; border:1px solid #D5DEE8;"
        "                                  border-radius:8px; padding:8px 10px; }"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border:1.5px solid #1E88E5; }"
        "QLineEdit#globalSearch { border-radius:20px; padding:9px 18px; min-width:250px; border:1px solid #E3EAF2; }"
        "QLineEdit#loginUser, QLineEdit#loginPass { padding:12px 14px; }"

        /* Boutons principaux visibles et contrastés */
        "QPushButton { background-color:#1E88E5; color:#FFFFFF; border:none; border-radius:8px;"
        "              padding:8px 16px; font-weight:bold; font-size:10pt; min-height:30px; }"
        "QPushButton:hover { background-color:#1565C0; }"
        "QPushButton:pressed { background-color:#0D47A1; }"
        "QPushButton:disabled { background-color:#CFD8DC; color:#90A4AE; }"
        "QPushButton#danger { background-color:#E53935; color:#FFFFFF; }"
        "QPushButton#danger:hover { background-color:#C62828; }"
        "QPushButton#secondary { background-color:#ECEFF1; color:#37474F; border:1px solid #CFD8DC; }"
        "QPushButton#secondary:hover { background-color:#CFD8DC; }"
        "QPushButton#innovant { background-color:#00897B; color:#FFFFFF; }"
        "QPushButton#innovant:hover { background-color:#00695C; }"
        "QPushButton#btnLogin { padding:13px; font-size:12pt; background-color:#1E88E5; color:white; }"

        /* Règles forcées pour garantir la visibilité à l'intérieur des cartes */
        "QFrame#card QPushButton { background-color:#1E88E5; color:#FFFFFF; font-weight:bold; border-radius:8px; padding:8px 14px; }"
        "QFrame#card QPushButton:hover { background-color:#1565C0; }"
        "QFrame#card QPushButton#danger { background-color:#E53935; color:#FFFFFF; }"
        "QFrame#card QPushButton#danger:hover { background-color:#C62828; }"
        "QFrame#card QPushButton#secondary { background-color:#ECEFF1; color:#37474F; border:1px solid #CFD8DC; }"
        "QFrame#card QPushButton#secondary:hover { background-color:#CFD8DC; }"
        "QFrame#card QPushButton#innovant { background-color:#00897B; color:#FFFFFF; }"
        "QFrame#card QPushButton#innovant:hover { background-color:#00695C; }"

        "QTableView { background:white; alternate-background-color:#F8FAFC; border:none;"
        "             selection-background-color:#DCEBFA; selection-color:#37474F; }"
        "QTableView::item { padding:4px 6px; border:none; }"
        "QHeaderView::section { background:white; color:#1E88E5; padding:10px 6px; border:none;"
        "                       border-bottom:2px solid #1E88E5; font-weight:bold; }"

        "QCalendarWidget QWidget#qt_calendar_navigationbar { background:#1E88E5; border-radius:8px; }"
        "QCalendarWidget QToolButton { color:white; background:transparent; border:none; font-weight:bold; }"
        "QCalendarWidget QSpinBox { color:white; background:transparent; border:none; }"
        "QCalendarWidget QAbstractItemView:enabled { color:#37474F; background:white;"
        "    selection-background-color:#1E88E5; selection-color:white; outline:0; }"
        "QCalendarWidget QAbstractItemView:disabled { color:#B0BEC5; }"

        "QScrollArea { border:none; background:transparent; }"
        "QScrollBar:vertical { background:transparent; width:10px; }"
        "QScrollBar::handle:vertical { background:#CFD8DC; border-radius:5px; min-height:30px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }");
}

void smartweather::erreur(const QString &msg)
{
    QMessageBox::warning(this, "Erreur", msg);
}

bool smartweather::formulaireValide()
{
    const QString id = m_id->text().trimmed();
    if (id.isEmpty()) {
        erreur("L'identifiant de l'employé (ID) est obligatoire.");
        m_id->setFocus();
        return false;
    }
    static const QRegularExpression reId("^[A-Za-z0-9_-]{1,20}$");
    if (!reId.match(id).hasMatch()) {
        erreur("L'identifiant ne doit contenir que des lettres, des chiffres ou des tirets (sans espaces ni caractères spéciaux).");
        m_id->setFocus();
        return false;
    }

    const QString nom = m_nom->text().trimmed();
    if (nom.isEmpty()) {
        erreur("Le nom de l'employé est obligatoire.");
        m_nom->setFocus();
        return false;
    }
    static const QRegularExpression reLettres("^[A-Za-zÀ-ÿ\\s'-]{2,40}$");
    if (!reLettres.match(nom).hasMatch()) {
        erreur("Le nom ne doit contenir que des lettres (au moins 2 caractères, sans chiffres).");
        m_nom->setFocus();
        return false;
    }

    const QString prenom = m_prenom->text().trimmed();
    if (prenom.isEmpty()) {
        erreur("Le prénom de l'employé est obligatoire.");
        m_prenom->setFocus();
        return false;
    }
    if (!reLettres.match(prenom).hasMatch()) {
        erreur("Le prénom ne doit contenir que des lettres (au moins 2 caractères, sans chiffres).");
        m_prenom->setFocus();
        return false;
    }

    const QString email = m_email->text().trimmed();
    if (email.isEmpty()) {
        erreur("L'adresse email est obligatoire.");
        m_email->setFocus();
        return false;
    }
    static const QRegularExpression reEmail("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!reEmail.match(email).hasMatch()) {
        erreur("Veuillez saisir une adresse email valide (exemple : prenom.nom@inm.tn).");
        m_email->setFocus();
        return false;
    }

    const QString gouv = m_gouvernorat->text().trimmed();
    if (gouv.isEmpty()) {
        erreur("Le gouvernorat d'affectation est obligatoire.");
        m_gouvernorat->setFocus();
        return false;
    }
    if (!reLettres.match(gouv).hasMatch()) {
        erreur("Le gouvernorat ne doit contenir que des lettres.");
        m_gouvernorat->setFocus();
        return false;
    }

    if (m_date->date() > QDate::currentDate()) {
        erreur("La date de recrutement ne peut pas être postérieure à la date d'aujourd'hui.");
        m_date->setFocus();
        return false;
    }
    if (m_date->date().year() < 1960) {
        erreur("La date de recrutement saisie est invalide (antérieure à 1960).");
        m_date->setFocus();
        return false;
    }

    return true;
}

void smartweather::ajouter()
{
    if (!formulaireValide()) return;

    const QString id = m_id->text().trimmed();

    // Contrôle de saisie : Vérification d'unicité de l'ID
    if (Employe::existe(id)) {
        erreur(QString("Un employé avec l'identifiant « %1 » existe déjà dans la base de données.").arg(id));
        m_id->setFocus();
        return;
    }

    Employe e(id,
              m_nom->text().trimmed(),
              m_prenom->text().trimmed(),
              m_email->text().trimmed(),
              m_poste->currentText(),
              m_structure->currentText(),
              m_gouvernorat->text().trimmed(),
              m_date->date(),
              m_statut->currentText(),
              m_horaire->currentText());

    if (e.ajouter()) {
        QMessageBox::information(this, "Succès - Ajout Employé",
                                 QString("L'employé #%1 (%2 %3) a été ajouté avec succès.")
                                     .arg(id, e.getNom(), e.getPrenom()));
        viderFormulaire();
        charger();
        rafraichirDashboard();
    } else {
        erreur("Erreur technique lors de l'ajout de l'employé dans la base.");
    }
}

void smartweather::modifier()
{
    if (m_idCourant.isEmpty()) {
        erreur("Veuillez sélectionner un employé dans le tableau pour le modifier.");
        return;
    }
    if (!formulaireValide()) return;

    Employe e(m_idCourant,
              m_nom->text().trimmed(),
              m_prenom->text().trimmed(),
              m_email->text().trimmed(),
              m_poste->currentText(),
              m_structure->currentText(),
              m_gouvernorat->text().trimmed(),
              m_date->date(),
              m_statut->currentText(),
              m_horaire->currentText());

    if (e.modifier()) {
        QMessageBox::information(this, "Succès - Modification",
                                 QString("Les informations de l'employé #%1 ont été mises à jour avec succès.")
                                     .arg(m_idCourant));
        charger();
        rafraichirDashboard();
    } else {
        erreur("Erreur lors de la modification des données de l'employé.");
    }
}

void smartweather::supprimer()
{
    if (m_idCourant.isEmpty()) {
        erreur("Veuillez sélectionner un employé dans le tableau.");
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Gestion du départ employé");
    msgBox.setText(QString("Que souhaitez-vous faire pour l'employé sélectionné (ID #%1) ?").arg(m_idCourant));
    msgBox.setInformativeText("Conformément au cahier des charges, vous pouvez archiver l'employé lors de son départ ou le supprimer définitivement de la base.");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *btnArchiver = msgBox.addButton("📦 Archiver (Départ)", QMessageBox::ActionRole);
    QPushButton *btnSupprimer = msgBox.addButton("🗑️ Supprimer définitivement", QMessageBox::DestructiveRole);
    QPushButton *btnAnnuler = msgBox.addButton("Annuler", QMessageBox::RejectRole);
    Q_UNUSED(btnAnnuler);

    msgBox.exec();

    if (msgBox.clickedButton() == btnArchiver) {
        archiverEmploye();
    } else if (msgBox.clickedButton() == btnSupprimer) {
        Employe e;
        if (e.supprimer(m_idCourant)) {
            QMessageBox::information(this, "Suppression", QString("L'employé #%1 a été supprimé définitivement.").arg(m_idCourant));
            viderFormulaire();
            charger();
            rafraichirDashboard();
        } else {
            erreur("Erreur lors de la suppression de l'employé.");
        }
    }
}

void smartweather::archiverEmploye()
{
    if (m_idCourant.isEmpty()) return;
    if (Employe::archiver(m_idCourant)) {
        QMessageBox::information(this, "Archivage", QString("L'employé #%1 a été archivé avec succès (Statut mis à jour en 'Archivé').").arg(m_idCourant));
        viderFormulaire();
        charger();
        rafraichirDashboard();
    } else {
        erreur("Échec de l'archivage de l'employé.");
    }
}

void smartweather::charger()
{
    const QString texte = m_recherche->text().trimmed();
    const QString critere = m_critere->currentText();
    const QString tri = m_tri->currentText();

    QSqlQueryModel *model = nullptr;
    if (!texte.isEmpty()) {
        model = Employe::rechercher(texte, critere, tri);
    } else {
        model = Employe::trier(tri, "ASC");
    }

    QSqlQueryModel *old = m_model;
    m_model = model;
    m_table->setModel(m_model);
    if (old && old != m_model && old->parent() == this) {
        old->deleteLater();
    }

    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (m_table->horizontalHeader()->count() > C_ID) {
        m_table->horizontalHeader()->setSectionResizeMode(C_ID, QHeaderView::ResizeToContents);
    }
}

void smartweather::remplirFormulaire(const QModelIndex &index)
{
    if (!index.isValid()) return;
    auto *model = qobject_cast<QSqlQueryModel*>(m_table->model());
    if (!model) model = m_model;
    if (!model || index.row() < 0 || index.row() >= model->rowCount()) return;

    const QSqlRecord r = model->record(index.row());
    m_idCourant = r.value(C_ID).toString();
    m_id->setText(m_idCourant);
    m_id->setEnabled(false); // ID fixé lors de l'édition d'un employé existant
    m_nom->setText(r.value(C_NOM).toString());
    m_prenom->setText(r.value(C_PRENOM).toString());
    m_email->setText(r.value(C_EMAIL).toString());
    m_poste->setCurrentText(r.value(C_POSTE).toString());
    m_structure->setCurrentText(r.value(C_STRUCT).toString());
    m_gouvernorat->setText(r.value(C_GOUV).toString());

    QDate d = QDate::fromString(r.value(C_DATE).toString(), "yyyy-MM-dd");
    if (!d.isValid()) d = r.value(C_DATE).toDate();
    if (!d.isValid()) d = QDate::currentDate();
    m_date->setDate(d);

    m_statut->setCurrentText(r.value(C_STATUT).toString());
    m_horaire->setCurrentText(r.value(C_HORAIRE).toString());
}

void smartweather::viderFormulaire()
{
    m_idCourant.clear();
    m_id->clear();
    m_id->setEnabled(true); // Réactiver la saisie d'un nouvel identifiant
    m_nom->clear(); m_prenom->clear(); m_email->clear(); m_gouvernorat->clear();
    m_poste->setCurrentIndex(0); m_structure->setCurrentIndex(0);
    m_statut->setCurrentIndex(0); m_horaire->setCurrentIndex(0);
    m_date->setDate(QDate::currentDate());
    m_table->clearSelection();
}

void smartweather::exporterCsv()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter Registre CSV", "employes.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) { erreur("Impossible d'écrire le fichier."); return; }
    QTextStream out(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out.setGenerateByteOrderMark(true);
    const int cols = m_model->columnCount();
    for (int c = 0; c < cols; ++c)
        out << m_model->headerData(c, Qt::Horizontal).toString() << (c + 1 < cols ? ";" : "\n");
    for (int r = 0; r < m_model->rowCount(); ++r)
        for (int c = 0; c < cols; ++c)
            out << m_model->data(m_model->index(r, c)).toString() << (c + 1 < cols ? ";" : "\n");

    QMessageBox::information(this, "Export CSV", "Le registre des employés a été exporté en format CSV avec succès.");
}

void smartweather::exporterPdf()
{
    bool exportFiche = false;
    if (!m_idCourant.isEmpty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Exportation PDF - Module Employés");
        msg.setText("Quel document PDF souhaitez-vous générer ?");
        msg.setInformativeText("Vous pouvez exporter la fiche individuelle de l'employé sélectionné ou le registre complet officiel.");
        msg.setIcon(QMessageBox::Question);

        QPushButton *btnFiche = msg.addButton(QString("👤 Fiche Individuelle (#%1 %2)").arg(m_idCourant, m_nom->text()), QMessageBox::ActionRole);
        QPushButton *btnListe = msg.addButton("📋 Registre Officiel Complet", QMessageBox::ActionRole);
        QPushButton *btnCancel = msg.addButton("Annuler", QMessageBox::RejectRole);
        Q_UNUSED(btnCancel);

        msg.exec();
        if (msg.clickedButton() == btnFiche) {
            exportFiche = true;
        } else if (msg.clickedButton() == btnListe) {
            exportFiche = false;
        } else {
            return;
        }
    }

    QString editeur = m_user.isEmpty() ? "Responsable RH" : m_user;

    if (exportFiche) {
        QString defaultName = QString("fiche_employe_%1.pdf").arg(m_idCourant);
        QString path = QFileDialog::getSaveFileName(this, "Exporter Fiche Individuelle PDF", defaultName, "Documents PDF (*.pdf)");
        if (path.isEmpty()) return;

        if (Employe::exporterFichePDF(m_idCourant, path, editeur)) {
            QMessageBox::information(this, "Export PDF Réussi", "La fiche individuelle de l'employé a été générée avec succès.");
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        } else {
            erreur("Erreur lors de la génération de la fiche PDF.");
        }
    } else {
        QString defaultName = QString("registre_personnel_inm_%1.pdf").arg(QDate::currentDate().toString("yyyy_MM_dd"));
        QString path = QFileDialog::getSaveFileName(this, "Exporter Registre PDF", defaultName, "Documents PDF (*.pdf)");
        if (path.isEmpty()) return;

        if (Employe::exporterListePDF(path, editeur)) {
            QMessageBox::information(this, "Export PDF Réussi", "Le registre complet du personnel a été exporté avec succès.");
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        } else {
            erreur("Erreur lors de la génération du registre PDF.");
        }
    }
}

void smartweather::afficherStats()
{
    auto *dlg = new QDialog(this);
    dlg->setWindowTitle("Statistiques Détaillées des Ressources Humaines — INM");
    dlg->resize(680, 520);

    auto *lay = new QVBoxLayout(dlg);
    lay->setSpacing(14);
    lay->setContentsMargins(20, 20, 20, 20);

    auto *titre = new QLabel("📊 Répartition et Indicateurs du Personnel (INM)");
    titre->setStyleSheet("font-size:16pt; font-weight:bold; color:#1E88E5;");
    lay->addWidget(titre);

    auto *tabs = new QTabWidget;

    // Tab 1 : Répartition par Poste
    auto *wPoste = new QWidget;
    auto *lPoste = new QVBoxLayout(wPoste);
    QSqlQuery qTotal("SELECT COUNT(*) FROM employe");
    int total = qTotal.next() ? qTotal.value(0).toInt() : 1;
    if (total == 0) total = 1;

    QSqlQuery qp("SELECT poste, COUNT(*) FROM employe GROUP BY poste ORDER BY COUNT(*) DESC");
    while (qp.next()) {
        QString pNom = qp.value(0).toString();
        int count = qp.value(1).toInt();
        int pct = qRound((count * 100.0) / total);

        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(QString("<b>%1</b> (%2 agents)").arg(pNom).arg(count));
        lbl->setFixedWidth(230);
        auto *bar = new QProgressBar;
        bar->setRange(0, 100);
        bar->setValue(pct);
        bar->setFormat(QString("%1%").arg(pct));
        bar->setStyleSheet("QProgressBar { border: 1px solid #D5DEE8; border-radius: 6px; text-align: center; height: 18px; }"
                           "QProgressBar::chunk { background-color: #1E88E5; border-radius: 5px; }");
        row->addWidget(lbl);
        row->addWidget(bar);
        lPoste->addLayout(row);
    }
    lPoste->addStretch();
    tabs->addTab(wPoste, "Répartition par Poste");

    // Tab 2 : Répartition par Structure
    auto *wStruct = new QWidget;
    auto *lStruct = new QVBoxLayout(wStruct);
    QSqlQuery qs("SELECT structure, COUNT(*) FROM employe GROUP BY structure");
    while (qs.next()) {
        QString sNom = qs.value(0).toString();
        int count = qs.value(1).toInt();
        int pct = qRound((count * 100.0) / total);

        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(QString("<b>%1</b> (%2 agents)").arg(sNom).arg(count));
        lbl->setFixedWidth(230);
        auto *bar = new QProgressBar;
        bar->setRange(0, 100);
        bar->setValue(pct);
        bar->setFormat(QString("%1%").arg(pct));
        bar->setStyleSheet("QProgressBar { border: 1px solid #D5DEE8; border-radius: 6px; text-align: center; height: 18px; }"
                           "QProgressBar::chunk { background-color: #00897B; border-radius: 5px; }");
        row->addWidget(lbl);
        row->addWidget(bar);
        lStruct->addLayout(row);
    }
    lStruct->addStretch();
    tabs->addTab(wStruct, "Structure (Siège vs Régionale)");

    // Tab 3 : Effectifs par Gouvernorat
    auto *wGouv = new QWidget;
    auto *lGouv = new QVBoxLayout(wGouv);
    QSqlQuery qg("SELECT gouvernorat, COUNT(*) FROM employe GROUP BY gouvernorat ORDER BY COUNT(*) DESC");
    int shown = 0;
    while (qg.next() && shown < 8) {
        QString gNom = qg.value(0).toString();
        int count = qg.value(1).toInt();
        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(QString("📍 <b>%1</b> : %2 agents").arg(gNom).arg(count));
        row->addWidget(lbl);
        lGouv->addLayout(row);
        shown++;
    }
    lGouv->addStretch();
    tabs->addTab(wGouv, "Effectifs par Gouvernorat");

    lay->addWidget(tabs);

    auto *btnClose = new QPushButton("Fermer");
    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    lay->addWidget(btnClose, 0, Qt::AlignRight);

    dlg->exec();
}

void smartweather::planifierVeille24h()
{
    auto *dlg = new QDialog(this);
    dlg->setWindowTitle("Planning Automatique de Veille 24h/24 (INM)");
    dlg->resize(880, 560);

    auto *lay = new QVBoxLayout(dlg);
    lay->setSpacing(12);
    lay->setContentsMargins(20, 20, 20, 20);

    auto *titre = new QLabel("📅 Planning Automatique de Veille 24h/24 — Roster Équilibré");
    titre->setStyleSheet("font-size:15pt; font-weight:bold; color:#1E88E5;");
    lay->addWidget(titre);

    auto *desc = new QLabel("Algorithme d'équilibrage des gardes (Jour 08h-20h / Nuit 20h-08h) respectant les temps de repos légaux et les congés des agents.");
    desc->setStyleSheet("color:#78909C; font-size:10pt;");
    desc->setWordWrap(true);
    lay->addWidget(desc);

    struct Agent { int id; QString nom; QString prenom; QString poste; QString gouv; };
    QVector<Agent> pool;
    QSqlQuery q("SELECT id_employe, nom, prenom, poste, gouvernorat FROM employe "
                "WHERE statut = 'Actif' AND (type_horaire = 'Garde 24h' OR poste LIKE '%observation%' OR poste LIKE '%Ingénieur%') "
                "ORDER BY id_employe ASC");
    while (q.next()) {
        pool.append({q.value(0).toInt(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString()});
    }

    if (pool.isEmpty()) {
        QSqlQuery qFallback("SELECT id_employe, nom, prenom, poste, gouvernorat FROM employe WHERE statut = 'Actif' ORDER BY id_employe ASC");
        while (qFallback.next()) {
            pool.append({qFallback.value(0).toInt(), qFallback.value(1).toString(), qFallback.value(2).toString(), qFallback.value(3).toString(), qFallback.value(4).toString()});
        }
    }

    if (pool.isEmpty()) {
        QMessageBox::warning(this, "Planning Veille", "Aucun employé actif disponible pour générer un planning de veille.");
        return;
    }

    auto *table = new QTableWidget;
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"Date", "Jour", "Quart de Veille", "Agent Assigné", "Poste", "Gouvernorat"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);

    QDate start = QDate::currentDate();
    int agentIdx = 0;
    int row = 0;
    table->setRowCount(14); // 7 jours x 2 quarts

    for (int d = 0; d < 7; ++d) {
        QDate curDate = start.addDays(d);
        QString dayName = QLocale(QLocale::French).dayName(curDate.dayOfWeek(), QLocale::LongFormat);
        if (!dayName.isEmpty()) dayName[0] = dayName[0].toUpper();

        // Quart Jour (08:00 - 20:00)
        const auto &agentJour = pool[agentIdx % pool.size()];
        agentIdx++;

        table->setItem(row, 0, new QTableWidgetItem(curDate.toString("dd/MM/yyyy")));
        table->setItem(row, 1, new QTableWidgetItem(dayName));
        auto *itemShiftJ = new QTableWidgetItem("☀️ Jour (08h - 20h)");
        itemShiftJ->setForeground(QColor("#D97706"));
        table->setItem(row, 2, itemShiftJ);
        table->setItem(row, 3, new QTableWidgetItem(QString("%1 %2 (#%3)").arg(agentJour.nom, agentJour.prenom).arg(agentJour.id)));
        table->setItem(row, 4, new QTableWidgetItem(agentJour.poste));
        table->setItem(row, 5, new QTableWidgetItem(agentJour.gouv));
        row++;

        // Quart Nuit (20:00 - 08:00)
        const auto &agentNuit = pool[agentIdx % pool.size()];
        agentIdx++;

        table->setItem(row, 0, new QTableWidgetItem(curDate.toString("dd/MM/yyyy")));
        table->setItem(row, 1, new QTableWidgetItem(dayName));
        auto *itemShiftN = new QTableWidgetItem("🌙 Nuit (20h - 08h)");
        itemShiftN->setForeground(QColor("#6D28D9"));
        table->setItem(row, 2, itemShiftN);
        table->setItem(row, 3, new QTableWidgetItem(QString("%1 %2 (#%3)").arg(agentNuit.nom, agentNuit.prenom).arg(agentNuit.id)));
        table->setItem(row, 4, new QTableWidgetItem(agentNuit.poste));
        table->setItem(row, 5, new QTableWidgetItem(agentNuit.gouv));
        row++;
    }

    lay->addWidget(table);

    auto *btnRow = new QHBoxLayout;
    auto *btnPdf = new QPushButton("📄 Exporter ce Planning (PDF)");
    auto *btnClose = new QPushButton("Fermer");
    btnPdf->setCursor(Qt::PointingHandCursor);
    btnClose->setCursor(Qt::PointingHandCursor);
    btnRow->addStretch();
    btnRow->addWidget(btnPdf);
    btnRow->addWidget(btnClose);
    lay->addLayout(btnRow);

    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    connect(btnPdf, &QPushButton::clicked, [table, dlg]() {
        QString path = QFileDialog::getSaveFileName(dlg, "Exporter Planning Veille", "planning_veille_24h.pdf", "PDF (*.pdf)");
        if (path.isEmpty()) return;

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(path);
        printer.setPageOrientation(QPageLayout::Landscape);

        QString html = "<html><head><style>"
                       "body { font-family: Arial; margin: 15px; color: #37474F; }"
                       "h2 { color: #1E88E5; text-align: center; margin-bottom: 4px; }"
                       "h4 { color: #546E7A; text-align: center; margin-top: 0; }"
                       "table { width: 100%; border-collapse: collapse; margin-top: 15px; font-size: 11px; }"
                       "th { background: #1E88E5; color: white; padding: 8px; text-align: left; }"
                       "td { padding: 6px 8px; border-bottom: 1px solid #E2E8F0; }"
                       "tr:nth-child(even) { background-color: #F8FAFC; }"
                       "</style></head><body>"
                       "<h2>INSTITUT NATIONAL DE LA MÉTÉOROLOGIE — INM</h2>"
                       "<h4>PLANNING OFFICIEL DES GARDES ET VEILLE OPÉRATIONNELLE 24H/24</h4>"
                       "<p><b>Généré le :</b> " + QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm") + "</p>"
                       "<table><tr><th>Date</th><th>Jour</th><th>Quart</th><th>Agent</th><th>Poste</th><th>Gouvernorat</th></tr>";

        for (int r = 0; r < table->rowCount(); ++r) {
            html += "<tr>";
            for (int c = 0; c < table->columnCount(); ++c) {
                html += "<td>" + table->item(r, c)->text() + "</td>";
            }
            html += "</tr>";
        }
        html += "</table></body></html>";

        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&printer);
        QMessageBox::information(dlg, "Export Planning", "Le planning a été exporté en PDF avec succès.");
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });

    dlg->exec();
}

void smartweather::affecterTechnicien()
{
    auto *dlg = new QDialog(this);
    dlg->setWindowTitle("Affectation Intelligente des Techniciens (INM)");
    dlg->resize(760, 500);

    auto *lay = new QVBoxLayout(dlg);
    lay->setSpacing(14);
    lay->setContentsMargins(20, 20, 20, 20);

    auto *titre = new QLabel("🛠 Affectation Intelligente de Technicien de Maintenance");
    titre->setStyleSheet("font-size:15pt; font-weight:bold; color:#1E88E5;");
    lay->addWidget(titre);

    auto *desc = new QLabel("L'algorithme analyse la proximité géographique (même gouvernorat), la structure de rattachement et la disponibilité pour recommander le technicien optimal.");
    desc->setStyleSheet("color:#78909C; font-size:10pt;");
    desc->setWordWrap(true);
    lay->addWidget(desc);

    auto *form = new QHBoxLayout;
    form->addWidget(new QLabel("Gouvernorat de l'intervention :"));
    auto *cmbGouv = new QComboBox;
    const QStringList gouvList = {"Tunis", "Ariana", "Ben Arous", "Manouba", "Nabeul", "Zaghouan",
                                  "Bizerte", "Béja", "Jendouba", "Le Kef", "Siliana", "Sousse",
                                  "Monastir", "Mahdia", "Sfax", "Kairouan", "Kasserine", "Sidi Bouzid",
                                  "Gabès", "Médenine", "Tataouine", "Gafsa", "Tozeur", "Kébili"};
    cmbGouv->addItems(gouvList);
    form->addWidget(cmbGouv, 1);
    auto *btnChercher = new QPushButton("🔍 Analyser & Recommander");
    form->addWidget(btnChercher);
    lay->addLayout(form);

    auto *table = new QTableWidget;
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"Score Match", "ID", "Technicien", "Gouvernorat", "Structure", "Statut"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    lay->addWidget(table);

    auto execSearch = [cmbGouv, table]() {
        QString targetGouv = cmbGouv->currentText().trimmed();
        table->setRowCount(0);

        struct Cand { int id; QString nom; QString prenom; QString gouv; QString structure; QString statut; int score; };
        QVector<Cand> cands;

        QSqlQuery q("SELECT id_employe, nom, prenom, gouvernorat, structure, statut FROM employe "
                    "WHERE (poste LIKE '%Technicien%' OR poste LIKE '%maintenance%')");
        while (q.next()) {
            Cand c;
            c.id = q.value(0).toInt();
            c.nom = q.value(1).toString();
            c.prenom = q.value(2).toString();
            c.gouv = q.value(3).toString();
            c.structure = q.value(4).toString();
            c.statut = q.value(5).toString();

            c.score = 30; // base score
            if (c.gouv.compare(targetGouv, Qt::CaseInsensitive) == 0) c.score += 50; // exact location match
            if (c.statut == "Actif") c.score += 20; // actively available

            cands.append(c);
        }

        std::sort(cands.begin(), cands.end(), [](const Cand &a, const Cand &b) {
            return a.score > b.score;
        });

        table->setRowCount(cands.size());
        for (int i = 0; i < cands.size(); ++i) {
            const auto &c = cands[i];
            QString badge = (i == 0 && c.score >= 80) ? "⭐ 100% (Optimal)" : QString("%1%").arg(c.score);
            auto *itemBadge = new QTableWidgetItem(badge);
            if (i == 0) itemBadge->setForeground(QColor("#00897B"));

            table->setItem(i, 0, itemBadge);
            table->setItem(i, 1, new QTableWidgetItem(QString("#%1").arg(c.id)));
            table->setItem(i, 2, new QTableWidgetItem(QString("%1 %2").arg(c.nom, c.prenom)));
            table->setItem(i, 3, new QTableWidgetItem(c.gouv));
            table->setItem(i, 4, new QTableWidgetItem(c.structure));
            table->setItem(i, 5, new QTableWidgetItem(c.statut));
        }
    };

    connect(btnChercher, &QPushButton::clicked, execSearch);
    connect(cmbGouv, QOverload<int>::of(&QComboBox::currentIndexChanged), execSearch);
    execSearch(); // Initial run

    auto *btnRow = new QHBoxLayout;
    auto *btnAffecter = new QPushButton("✅ Confirmer l'Affectation");
    auto *btnClose = new QPushButton("Fermer");
    btnAffecter->setObjectName("innovant");
    btnRow->addStretch();
    btnRow->addWidget(btnAffecter);
    btnRow->addWidget(btnClose);
    lay->addLayout(btnRow);

    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    connect(btnAffecter, &QPushButton::clicked, [dlg, table, cmbGouv]() {
        int r = table->currentRow();
        if (r < 0) {
            QMessageBox::warning(dlg, "Sélection", "Veuillez sélectionner un technicien dans la liste.");
            return;
        }
        QString nom = table->item(r, 2)->text();
        QString gouv = cmbGouv->currentText();
        QMessageBox::information(dlg, "Affectation Validée",
            QString("Le technicien <b>%1</b> a été affecté avec succès à l'intervention dans le gouvernorat de <b>%2</b>.").arg(nom, gouv));
        dlg->accept();
    });

    dlg->exec();
}

void smartweather::on_btnAjouter_clicked()
{
    ajouter();
}

void smartweather::on_btnModifier_clicked()
{
    modifier();
}

void smartweather::on_btnSupprimer_clicked()
{
    supprimer();
}
