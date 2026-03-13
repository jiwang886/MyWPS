#include "childwnd.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTextListFormat>
#include <QTextBlockFormat>
#include <QtWidgets>
ChildWnd::ChildWnd()
{
    setAttribute(Qt::WA_DeleteOnClose); //子窗口关闭，销毁该类的实例对象

    m_bSaved = false;
}
void ChildWnd::newDoc()
{
    static int wndSeqNum = 1;
    m_CurDocPath = QString("WPS 文档 %1").arg(wndSeqNum++);
    //设置窗体标题,文档改动名称后面加“*”
    setWindowTitle(m_CurDocPath + "[*]" + " - MyWPS");
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(docBeModified()));   //判断文档是否被修改过
}

void ChildWnd::docBeModified()
{
    setWindowModified(document()->isModified());            //将文档的修改状态反映到窗口标题

}

QString ChildWnd::getCurDocName()
{
    return QFileInfo(m_CurDocPath).fileName();
}

bool ChildWnd::loadDoc(const QString& docName)
{
    if(!docName.isEmpty())      //字符串不为空
    {
        QFile file(docName);    //确定文件地址
        if(!file.exists()) return false;        //文件存在
        if(!file.open(QFile::ReadOnly)) return false;   //文件有内容
        QByteArray text = file.readAll();
        if(Qt::mightBeRichText(text))
            setHtml(text);                          //有富文本
        else
            setPlainText(text);                     //普通文本
        setCurDoc(docName);
        connect(document(),SIGNAL(contentsChanged()),this,SLOT(docBeModified()));
        return true;
    }
}
void ChildWnd::setCurDoc(const QString& docName)
{

    m_CurDocPath = QFileInfo(docName).canonicalFilePath();      //返回绝对路径
    m_bSaved = true;                            //文档已被保存
    document()->setModified(false);             //文档内容没改动
    setWindowModified(0);                       //不显示改动
    setWindowTitle(getCurDocName() + "[*]");    //设置子窗口标题
}

bool ChildWnd::saveDoc()
{
    if(m_bSaved) return saveDocOpt(m_CurDocPath);
    else return saveAsDoc();
}
bool ChildWnd::saveAsDoc()
{
    QString docName = QFileDialog::getSaveFileName(this,"另存为",m_CurDocPath,"HTML文档(*.html);;所有文件(*.*)");
    if(docName.isEmpty()) return 0;
    else return saveDocOpt(docName);
}
bool ChildWnd::saveDocOpt(QString docName)
{
    if(!(docName.endsWith(".htm",Qt::CaseInsensitive)||docName.endsWith(".html",Qt::CaseInsensitive)))           //判断文件后缀，大小写不敏感
    {
        docName += ".html";     //添加后缀
    }
    QTextDocumentWriter writer(docName);                     //写入文本类
    bool isSuccess = writer.write(this->document());         //写入内容
    if(isSuccess) setCurDoc(docName);
    return isSuccess;
}
bool ChildWnd::promptSave()
{
    if(!document()->isModified()) return 1;

    QMessageBox::StandardButton result;             //返回按钮
    result = QMessageBox::warning(this,QString("提示"),QString("文档%1已修改，是否保存").arg(getCurDocName()),QMessageBox::Yes | QMessageBox::Discard | QMessageBox::No);
    if(result == QMessageBox::Yes)
        return saveDoc();
    else if(result == QMessageBox::No)
        return false;
    else
        return true;

}
void ChildWnd::closeEvent(QCloseEvent *event)
{
    if(promptSave())
        event->accept();
    else
        event->ignore();
}

void ChildWnd::setFormatOnSelectedWord(const QTextCharFormat & fmt)
{
    QTextCursor tcursor = textCursor();     // 是获取当前文本编辑器中光标对象的常用方法
    if(tcursor.hasSelection()) tcursor.select(QTextCursor::WordUnderCursor);    // Qt 中的一个枚举值，用于选中光标所在的整个单词

    tcursor.mergeCharFormat(fmt);   //光标选中的文本	只修改选中的部分
    mergeCurrentCharFormat(fmt);    //当前光标位置的格式	设置后续输入文字的格式

}
void ChildWnd::setALignOfDocumentText(int aligntype)
{
    if(aligntype == 1)
        setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);        //左对齐，并且绝对
    else if(aligntype == 2)
        setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if(aligntype == 3)
        setAlignment(Qt::AlignCenter | Qt::AlignAbsolute);          //居中
    else if(aligntype == 4)
        setAlignment(Qt::AlignJustify | Qt::AlignAbsolute);         //两端

}

void ChildWnd::setParaSyle(int pStyle)                  //项目标准
{
    QTextCursor tcursor = textCursor();                 //获取光标信息
    QTextListFormat::Style sname;
    if(pStyle !=0 )
    {
        switch(pStyle)
        {
        case 1:
            sname = QTextListFormat::ListDisc;          //表示黑色实心圆
            break;
        case 2:
            sname = QTextListFormat::ListCircle;          //表示空心圆
            break;
        case 3:
            sname = QTextListFormat::ListSquare;          //表示方形
            break;
        case 4:
            sname = QTextListFormat::ListDecimal;          //表示十进制整数
            break;
        case 5:
            sname = QTextListFormat::ListLowerAlpha;          //表示小写字母
            break;
        case 6:
            sname = QTextListFormat::ListUpperAlpha;          //表示大写字母
            break;
        case 7:
            sname = QTextListFormat::ListLowerRoman;          //表示小写罗马字母
            break;
        case 8:
            sname = QTextListFormat::ListUpperRoman;          //表示大写罗马字母
            break;
        default:
            sname = QTextListFormat::ListDisc;                  //默认
        }
        tcursor.beginEditBlock();                               //更新编辑开始
        QTextBlockFormat tBlockFmt = tcursor.blockFormat();     //获得文本块格式
        QTextListFormat tListFmt;
        if(tcursor.currentList())
        {
            tListFmt = tcursor.currentList()->format();
        }else{
            tListFmt.setIndent(tBlockFmt.indent() + 1);     //增加缩进
            tBlockFmt.setIndent(0);
            tcursor.setBlockFormat(tBlockFmt);
        }

        tListFmt.setStyle(sname);                       //根据获取改风格
        tcursor.createList(tListFmt);                   //放入链表中

        tcursor.endEditBlock();                                 //编辑结束
    }
    else
    {
        QTextBlockFormat tbfmt;
        tbfmt.setObjectIndex(-1);
        tcursor.mergeBlockFormat(tbfmt);
    }
}
