#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "childwnd.h"
#include <QMainWindow>
#include <QSignalMapper>
#include <QMdiSubWindow>
#include <QtPrintSupport/QPrintPreviewDialog>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initMainWindow();

    void docNew();

    void docOpen();

    void docSave();
    void docSaveAs();

    void docUndo();
    void docRedo();
    void docCut();
    void docCopy();
    void docPaste();

    void textBold();
    void textItalic();
    void textUnderLine();

    void textFamily(const QString &fmly);
    void textSize(const QString &ps);
    void textColor();

    void paraStyle(int nStyle);

    void docPrint();                   //打印
    void docPrintPre();                 //预览

    void printPreview(QPrinter* printer);   //预览
private slots:
    void on_action_N_triggered();
    void refreshMenus();
    void addSubWndListMenu();
    void on_action_O_2_triggered();

    void on_action_A_2_triggered();

    void on_action_T_2_triggered();

    void on_action_C_4_triggered();

    void on_action_X_triggered();

    void on_action_V_triggered();

    void setActiveSubWindow(QObject*);
    void on_action_O_triggered();

    void on_action_S_triggered();

    void on_action_A_triggered();

    void on_action_U_triggered();

    void on_action_R_triggered();

    void on_action_T_triggered();

    void on_action_C_triggered();

    void on_action_P_2_triggered();

    void on_action_B_triggered();

    void on_action_I_triggered();

    void on_action_U_2_triggered();

    void on_comboBox_2_activated(int index);


    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_action_L_triggered();

    void on_action_R_2_triggered();

    void on_action_C_3_triggered();

    void on_action_J_triggered();

    void on_action_C_2_triggered();

    void on_comboBox_activated(int index);

    void on_action_P_triggered();

    void on_action_triggered();

protected:
    void closeEvent(QCloseEvent *e);
private:
    Ui::MainWindow *ui;
    QSignalMapper * m_WndMapper;        //信号映射器
private:
    void formatEnable();
    ChildWnd * activateChildWnd();
    QMdiSubWindow * findChildWnd(const QString& docName);

};
#endif // MAINWINDOW_H
