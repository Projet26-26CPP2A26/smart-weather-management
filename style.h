#pragma once

// Theme clair moderne (bleu)
static const char *APP_STYLE = R"(
* { font-family: "Segoe UI", "Roboto", sans-serif; font-size: 13px; color: #1f2d3d; }
QMainWindow, QDialog, QWidget#central { background: #f4f7fb; }

QLabel#title    { font-size: 22px; font-weight: 700; color: #0d47a1; }
QLabel#subtitle { color: #6b7c93; }

QFrame#card { background: white; border: 1px solid #e3e9f2; border-radius: 12px; }
QLabel#kpiValue { font-size: 26px; font-weight: 700; color: #1565c0; }
QLabel#kpiLabel { color: #6b7c93; }

QPushButton {
    background: white; border: 1px solid #cfd8e6; border-radius: 8px;
    padding: 8px 14px;
}
QPushButton:hover { background: #eaf2ff; border-color: #90b4f0; }
QPushButton#primary { background: #1976d2; color: white; border: none; font-weight: 600; }
QPushButton#primary:hover { background: #1565c0; }
QPushButton#primary { color: white; }
QPushButton#danger { color: #c62828; border-color: #f0b4b4; }
QPushButton#danger:hover { background: #fdecec; }
QPushButton#accent { background: #00897b; color: white; border: none; font-weight: 600; }
QPushButton#accent:hover { background: #00695c; }

QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox, QTextEdit {
    background: white; border: 1px solid #cfd8e6; border-radius: 8px; padding: 6px 8px;
}
QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QTextEdit:focus { border: 1px solid #1976d2; }

QTableView {
    background: white; alternate-background-color: #f8fafd;
    border: 1px solid #e3e9f2; border-radius: 10px; gridline-color: transparent;
    selection-background-color: #bbdefb; selection-color: #0d1b2a;
}
QHeaderView::section {
    background: #e8f0fb; color: #0d47a1; font-weight: 600;
    padding: 8px; border: none; border-right: 1px solid #d5e1f3;
}

QTabWidget::pane { border: none; }
QTabBar::tab {
    background: transparent; padding: 10px 22px; margin-right: 4px;
    border-bottom: 3px solid transparent; font-weight: 600; color: #6b7c93;
}
QTabBar::tab:selected { color: #1565c0; border-bottom: 3px solid #1976d2; }

QStatusBar { background: white; color: #6b7c93; }
)";
