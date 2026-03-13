#ifndef CHILDWND_H
#define CHILDWND_H

#include <QTextEdit>

class ChildWnd : public QTextEdit
{
    Q_OBJECT
public:
    ChildWnd();
    QString m_CurDocPath;       //当前文档路径
    void newDoc();              //新建
    QString getCurDocName();    //获取文档名
    bool loadDoc(const QString& docName);       //打开文件
    void setCurDoc(const QString& docName);     //设置文档名字
    bool saveDoc();
    bool saveAsDoc();
    bool saveDocOpt(QString docName);

    void setFormatOnSelectedWord(const QTextCharFormat & fmt);

    void setALignOfDocumentText(int aligntype);

    void setParaSyle(int pStyle);
protected:
    void closeEvent(QCloseEvent *event);

private:
    bool m_bSaved;          //文档是否保存
    bool promptSave();        //尝试保存，用于提示用户需要保存修改

private slots:
    void docBeModified();       //文档修改时，窗口的标题栏加'*


};

#endif // CHILDWND_H
