#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFontDatabase>
#include <QObject>
#include <QMdiSubWindow>
#include <QMenu>
#include <QCloseEvent>
#include <QFileDialog>
#include <QActionGroup>
#include <QColorDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
}

void MainWindow::initMainWindow()
{
    //初始化字号列表项
    QFontDatabase fontdb;
    foreach(int fontsize,fontdb.standardSizes())        //fontdb.standardSizes()返回整数列表,用fontsize存储
        ui->comboBox_2->addItem(QString::number(fontsize));  //把字号大小存进去
    // QList<int> standardSizes = fontdb.standardSizes();
    // foreach(int fontSize, standardSizes) {
    //     ui->comboBox_2->addItem(QString::number(fontSize));
    // }
    QFont defFont;              //当前默认字体
    QString sFontsize;          //当前字号大小
    int defFontSize;            //当前默认字体的字号
    int defFontindex;           //当前字号在框中的索引号

    defFont = QApplication::font();
    defFontSize = defFont.pointSize();
    sFontsize = QString::number(defFontSize);
    defFontindex = ui->comboBox_2->findText("sFontsize");      //保存查找当前文本


    ui->comboBox_2->setCurrentText(QString::number(defFontindex));
    //设置多文档区域的滚动条
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    refreshMenus();         //刷新菜单
    connect(ui->mdiArea,&QMdiArea::subWindowActivated,this,&MainWindow::refreshMenus); //窗口改变则发送信号
    addSubWndListMenu();
    connect(ui->menu,&QMenu::aboutToShow,this,&MainWindow::addSubWndListMenu);


    //信号映射器构造
    m_WndMapper = new QSignalMapper(this);
    connect(m_WndMapper,SIGNAL(mappedObject(QObject*)),
            this, SLOT(setActiveSubWindow(QObject*)));

    //创建分组，将段落设置隔开，保证互斥性，一次选一种
    QActionGroup *alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->action_L);
    alignGroup->addAction(ui->action_R_2);
    alignGroup->addAction(ui->action_C_3);
    alignGroup->addAction(ui->action_J);
}

void MainWindow::docNew()
{
    ChildWnd * childWnd = new ChildWnd;
    ui->mdiArea->addSubWindow(childWnd);
    connect(childWnd,SIGNAL(copyAvailable(bool)),ui->action_T,SLOT(setEnabled(bool)));
    connect(childWnd,SIGNAL(copyAvailable(bool)),ui->action_C,SLOT(setEnabled(bool)));

    childWnd->newDoc();
    childWnd->show();
    formatEnable();
}
void MainWindow::formatEnable()
{
    ui->action_B->setEnabled(1);            //加粗
    ui->action_I->setEnabled(1);            //倾斜
    ui->action_U_2->setEnabled(1);          //下划线
    ui->action_R_2->setEnabled(1);          //右对齐
    ui->action_L->setEnabled(1);            //左对齐
    ui->action_C_3->setEnabled(1);          //居中
    ui->action_J->setEnabled(1);            //两端对齐
    ui->action_C_2->setEnabled(1);          //颜色
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_N_triggered()
{
    docNew();           //新建调用
}


void MainWindow::refreshMenus()                 //刷新菜单
{
    bool hasChild = (activateChildWnd() != nullptr);
    ui->action_S->setEnabled(hasChild);         //保存
    ui->action_A->setEnabled(hasChild);         //另存为
    ui->action_P->setEnabled(hasChild);         //打印
    ui->action->setEnabled(hasChild);           //预览
    ui->action_P_2->setEnabled(hasChild);       //粘贴
    ui->action_O_2->setEnabled(hasChild);       //关闭
    ui->action_A_2->setEnabled(hasChild);       //关闭所有
    ui->action_T_2->setEnabled(hasChild);       //平铺
    ui->action_C_4->setEnabled(hasChild);       //层叠
    ui->action_X->setEnabled(hasChild);         //下一个
    ui->action_V->setEnabled(hasChild);         //上一个
    //判断用户是否选中文本内容
    bool hasSelect = false;
    if (auto childWnd = activateChildWnd()) {  // C++17 if语句中定义变量
        hasSelect = childWnd->textCursor().hasSelection();
    }
    ui->action_T->setEnabled(hasSelect);            //剪切
    ui->action_C->setEnabled(hasSelect);            //复制
    ui->action_B->setEnabled(hasSelect);            //加粗
    ui->action_I->setEnabled(hasSelect);            //倾斜
    ui->action_U_2->setEnabled(hasSelect);          //下划线
    ui->action_R_2->setEnabled(hasSelect);          //右对齐
    ui->action_L->setEnabled(hasSelect);            //左对齐
    ui->action_C_3->setEnabled(hasSelect);          //居中
    ui->action_J->setEnabled(hasSelect);            //两端对齐
    ui->action_C_2->setEnabled(hasSelect);          //颜色

}
ChildWnd * MainWindow::activateChildWnd()       //获取活动子窗口
{
    QMdiSubWindow * actWnd = ui->mdiArea->activeSubWindow();  //获取当前子窗口
    if(actWnd)
        return qobject_cast<ChildWnd*>(actWnd->widget());
    else
        return 0;
}


void MainWindow::addSubWndListMenu()            //对窗口菜单添加
{
    ui->menu->clear();
    ui->menu->addAction(ui->action_O_2);
    ui->menu->addAction(ui->action_A_2);
    ui->menu->addSeparator();
    ui->menu->addAction(ui->action_T_2);
    ui->menu->addAction(ui->action_C_4);
    ui->menu->addSeparator();
    ui->menu->addAction(ui->action_X);
    ui->menu->addAction(ui->action_V);

    QList<QMdiSubWindow*> wnds = ui->mdiArea->subWindowList();    //将窗口链表进行保存
    if(!wnds.isEmpty()) ui->menu->addSeparator();           //加上分隔线

    for(int i =0 ;i<wnds.size();++i)
    {
        ChildWnd* childWnd = qobject_cast<ChildWnd*>(wnds.at(i)->widget());
        QString menuitem_text;
        menuitem_text = QString("%1 %2").arg(i+1).arg(childWnd->getCurDocName());   //获取文档名
        QAction *menuitem_act = ui->menu->addAction(menuitem_text);
        menuitem_act->setCheckable(1);
        menuitem_act->setChecked(childWnd == activateChildWnd());

        connect(menuitem_act,SIGNAL(triggered(bool)),m_WndMapper,SLOT(map()));
        m_WndMapper->setMapping(menuitem_act,wnds.at(i));
    }
    formatEnable();
}






void MainWindow::on_action_O_2_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}


void MainWindow::on_action_A_2_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}


void MainWindow::on_action_T_2_triggered()
{
    ui->mdiArea->tileSubWindows();
}



void MainWindow::on_action_C_4_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}


void MainWindow::on_action_X_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}


void MainWindow::on_action_V_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    ui->mdiArea->closeAllSubWindows();
    if(ui->mdiArea->currentSubWindow())
        e->ignore();            //忽略
    else
        e->accept();            //接受
}

void MainWindow::setActiveSubWindow(QObject*wnd)        //设置活动窗口
{
    if(wnd)
        ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(wnd));
}

void MainWindow::on_action_O_triggered()
{
    docOpen();
}

void MainWindow::docOpen()      //打开文件
{
    QString docName = QFileDialog::getOpenFileName(this,"打开文档","","文本文件(*.txt);;HTML文件(*.html *.htm);;所有文件(*.*)");
    if(!docName.isEmpty())
    {
        QMdiSubWindow *existWnd = findChildWnd(docName);
        if(existWnd)
        {
            ui->mdiArea->setActiveSubWindow(existWnd);
            return;
        }
        ChildWnd * childWnd = new ChildWnd;
        ui->mdiArea->addSubWindow(childWnd);

        connect(childWnd,SIGNAL(copyAvailable(bool)),ui->action_T,SLOT(setEnabled(bool)));
        connect(childWnd,SIGNAL(copyAvailable(bool)),ui->action_C,SLOT(setEnabled(bool)));


        if(childWnd->loadDoc(docName))
        {
            statusBar()->showMessage("文档已打开",3000);
            childWnd->show();
            formatEnable();
        }
        else
            childWnd->close();
    }
}

QMdiSubWindow * MainWindow::findChildWnd(const QString& docName)
{
    QString strFile = QFileInfo(docName).canonicalFilePath();
    foreach(QMdiSubWindow * subWnd,ui->mdiArea->subWindowList())
    {
        ChildWnd * chilWnd = qobject_cast<ChildWnd*>(subWnd->widget());
        if(chilWnd->m_CurDocPath == strFile)  return subWnd;
    }
    return 0;
}


void MainWindow::docSave()
{
    if(activateChildWnd()&&activateChildWnd()->saveDoc())
        statusBar()->showMessage("保存成功",3000);
}
void MainWindow::docSaveAs()
{
    if(activateChildWnd()&&activateChildWnd()->saveAsDoc())
        statusBar()->showMessage("另存成功",3000);
}
void MainWindow::on_action_S_triggered()        //保存
{
    docSave();
}


void MainWindow::on_action_A_triggered()        //另存为
{
    docSaveAs();
}

void MainWindow::docUndo()      //撤销
{
    if(activateChildWnd())
        activateChildWnd()->undo();
}
void MainWindow::docRedo()      //重做
{
    if(activateChildWnd())
        activateChildWnd()->redo();

}
void MainWindow::docCut()       //剪切
{
    if(activateChildWnd())
        activateChildWnd()->cut();
}
void MainWindow::docCopy()      //复制
{
    if(activateChildWnd())
        activateChildWnd()->copy();
}
void MainWindow::docPaste()     //粘贴
{
    if(activateChildWnd())
        activateChildWnd()->paste();
}

void MainWindow::on_action_U_triggered()
{
    docUndo();
}


void MainWindow::on_action_R_triggered()
{
    docRedo();
}


void MainWindow::on_action_T_triggered()
{
    docCut();
}


void MainWindow::on_action_C_triggered()
{
    docCopy();
}


void MainWindow::on_action_P_2_triggered()
{
    docPaste();
}

void MainWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->action_B->isChecked() ? QFont::Bold : QFont::Normal);
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectedWord(fmt);
}

void MainWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->action_I->isChecked());
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectedWord(fmt);
}
void MainWindow::textUnderLine()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->action_U_2->isChecked());
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectedWord(fmt);
}
void MainWindow::on_action_B_triggered()        //需要主动在ui勾选checkable
{
    textBold();
}


void MainWindow::on_action_I_triggered()         //需要主动在ui勾选checkable
{
    textItalic();
}


void MainWindow::on_action_U_2_triggered()       //需要主动在ui勾选checkable
{
    textUnderLine();
}

void MainWindow::textFamily(const QString &fmly)        //字体
{
    QTextCharFormat fmt;
    fmt.setFontFamily(fmly);
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectedWord(fmt);
}
void MainWindow::textSize(const QString &ps)            //字体大小
{
    qreal pointSize = ps.toFloat();                     //转换类型
    if(ps.toFloat()>0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        if(activateChildWnd())
            activateChildWnd()->setFormatOnSelectedWord(fmt);
    }

}



void MainWindow::on_comboBox_2_activated(int index)
{
    textSize(QString::number(index));
}



void MainWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QString fontFamily = f.family();
    textFamily(fontFamily);
}


void MainWindow::on_action_L_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setALignOfDocumentText(1);
}


void MainWindow::on_action_R_2_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setALignOfDocumentText(2);
}



void MainWindow::on_action_C_3_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setALignOfDocumentText(3);
}


void MainWindow::on_action_J_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setALignOfDocumentText(4);
}

void MainWindow::textColor()            //设置颜色
{
    if(activateChildWnd())
    {
        QColor color = QColorDialog::getColor(activateChildWnd()->textColor(),this);        //当前文本颜色
        if(!color.isValid()) return;        //判断合法性
        QTextCharFormat fmt;                //创建字符格式对象的语句，用于设置文本的样式（字体、颜色、大小等）
        fmt.setForeground(color);       //设置前景色
        activateChildWnd()->setFormatOnSelectedWord(fmt);

        QPixmap pix(16,16);
        pix.fill(color);
        ui->action_C_2->setIcon(pix);
    }
}
void MainWindow::on_action_C_2_triggered()
{
    textColor();
}
void MainWindow::paraStyle(int nStyle)          //选择标准
{
    if(activateChildWnd())
    {
        activateChildWnd()->setParaSyle(nStyle);
    }
}

void MainWindow::on_comboBox_activated(int index)
{
    paraStyle(index);
}

void MainWindow::docPrint()                  //打印
{
    QPrinter pter(QPrinter::HighResolution);        //打印像素
    QPrintDialog *ddlg = new QPrintDialog(&pter,this);
    if(activateChildWnd())
        ddlg->setOption(QAbstractPrintDialog::PrintSelection,true);     //保留设置
    ddlg->setWindowTitle("打印文档");

    ChildWnd* childWnd = activateChildWnd();
    if(ddlg->exec() == QDialog::Accepted)
        childWnd->print(&pter);
    delete ddlg;
}
void MainWindow::docPrintPre()                 //预览
{
    QPrinter pter;
    QPrintPreviewDialog preview(&pter,this);
    //connect(&preview,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    connect(&preview,&QPrintPreviewDialog::paintRequested,this,&MainWindow::printPreview);
    preview.exec();
}

void MainWindow::printPreview(QPrinter* printer)
{
    activateChildWnd()->print(printer);
}
void MainWindow::on_action_P_triggered()
{
    docPrint();
}


void MainWindow::on_action_triggered()
{
    docPrintPre();
}

