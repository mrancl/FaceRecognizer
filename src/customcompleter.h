#ifndef CUSTOMCOMPLETER_H
#define CUSTOMCOMPLETER_H

#include <QLineEdit>
#include <QStringList>
#include <QStringListModel>
#include <QString>
#include <QCompleter>

class CustomCompleter : public QCompleter
{
    Q_OBJECT
public:
    inline CustomCompleter(const QStringList& words, QObject * parent) :
    QCompleter(parent), m_list(words), m_model()
    {
        setModel(&m_model);
    }

inline void update(QString word)
{
    QStringList filtered = m_list.filter(word, caseSensitivity());
    m_model.setStringList(filtered);
    m_word = word;
    complete();
}

inline QString word()
{
    return m_word;
}

private:
    QStringList m_list;
    QStringListModel m_model;
    QString m_word;

};

#endif // CUSTOMCOMPLETER_H
