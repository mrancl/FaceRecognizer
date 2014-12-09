#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QLineEdit>
#include <QAbstractItemView>
#include <QKeyEvent>

#include "customcompleter.h"

class QToolButton;

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget *parent = NULL);

    void setCompleter(CustomCompleter *c);
    CustomCompleter *completer() const;

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void updateSearchButton(const QString &text);
    void insertCompletion(const QString &completion);

private:
    QString styleSheetForCurrentState() const;
    QString buttonStyleSheetForCurrentState() const;

    QToolButton *mSearchButton;
    CustomCompleter *c;
};

#endif // SEARCHLINEEDIT_H
