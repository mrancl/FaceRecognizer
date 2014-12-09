#include "actionbar.h"
#include <QIcon>
#include <QFontMetrics>
#include <QFont>
#include <QApplication>
#include <QStyleOption>
#include <QPainter>

AndroidActionBar::AndroidActionBar(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:lightGray;");

    // Create layout
    layout=new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSizeConstraint(QLayout::SetNoConstraint);

    // App Icon and Up Button
    appIcon = new QToolButton(this);
    appIcon->setIcon(QIcon(":/icons/app"));
    appIcon->setAutoRaise(true);
    appIcon->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(appIcon);

    // View Control Button
    viewControl = new QToolButton(this);
    if (QApplication::applicationDisplayName().isEmpty())
    {
        viewControl->setText("NoName");
    }
    else
    {
        viewControl->setText(QApplication::applicationDisplayName());
    }
    viewControl->setAutoRaise(true);
    viewControl->setFocusPolicy(Qt::NoFocus);
    viewControl->setPopupMode(QToolButton::InstantPopup);
    viewControl->setToolButtonStyle(Qt::ToolButtonTextOnly);
    viewControl->setStyleSheet("font:bold; height:1.5em;");
    viewMenu = new QMenu(this);
    viewMenu->raise();
    viewMenu->setStyle(&menuStyle); // needed because the icon size cannot be set by a StyleSheet
    viewMenu->setStyleSheet(
                "QMenu::item {padding: 0.3em 1.5em 0.3em 1.5em; border: 1px solid transparent;}"
                "QMenu::item::selected {border-color: black}");
    layout->addWidget(viewControl);

    // Spacer
    layout->addStretch();

    // Action Overflow Button
    overflowButton=new QToolButton(this);
    overflowButton->setIcon(QIcon(":/icons/overflow"));
    overflowButton->setToolTip(tr("more"));
    overflowButton->setAutoRaise(true);
    overflowButton->setFocusPolicy(Qt::NoFocus);
    overflowButton->setPopupMode(QToolButton::InstantPopup);
    layout->addWidget(overflowButton);

    // Action Overflow Menu
    overflowMenu=new QMenu(this);
    overflowMenu->setStyle(&menuStyle); // needed because the icon size cannot be set by a StyleSheet
    overflowMenu->setStyleSheet(
                "QMenu::item {padding: 0.3em 1.5em 0.3em 1.5em; border: 1px solid transparent;}"
                "QMenu::item::selected {border-color: black}");
    overflowButton->setMenu(overflowMenu);
}

AndroidActionBar::~AndroidActionBar() {}

void AndroidActionBar::resizeEvent(QResizeEvent* event)
{
    int oldWidth=event->oldSize().width();
    int newWidth=event->size().width();
    if (oldWidth!=newWidth)
    {
        adjustContent();
    }
}

void AndroidActionBar::paintEvent(QPaintEvent*)
{
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

void AndroidActionBar::setTitle(const QString& title, bool showUpButton)
{
    viewControl->setText(title);
    if (showUpButton)
    {
        appIcon->setIcon(QIcon(":/icons/app_up"));
        appIcon->setToolTip(tr("up"));
        connect(appIcon, &QToolButton::clicked, this, &AndroidActionBar::appIconClicked);
    }
    else
    {
        appIcon->setIcon(QIcon(":/icons/app"));
        appIcon->setToolTip("");
        disconnect(appIcon, &QToolButton::clicked, this, &AndroidActionBar::appIconClicked);
    }
    adjustContent();
}

void AndroidActionBar::appIconClicked()
{
    emit up();
}

void AndroidActionBar::adjustContent()
{
    // Get size of one em (text height in pixels)
    int em = fontMetrics().height();

    // update size of app icon and overflow menu button
    appIcon->setIconSize(QSize(2*em,2*em));
    overflowButton->setIconSize(QSize(2*em,2*em));

    // Check if all action buttons fit into the available space with text beside icon.
    bool needOverflow = false;
    int space = width() - appIcon->sizeHint().width() - viewControl->sizeHint().width();
    for (int i=0; i<buttonActions.size(); i++)
    {
        QAction* action = buttonActions.at(i);
        QToolButton* button = actionButtons.at(i);
        if (action->isVisible())
        {
            button->setIconSize(QSize(2*em,2*em));
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            space -= button->sizeHint().width();
        }
    }
    if (space < 0) {
        // Not enough space.
        // Check if all action buttons fit into the available space without text.
        int space = width() - appIcon->sizeHint().width() - viewControl->sizeHint().width();
        for (int i=0; i<buttonActions.size(); i++)
        {
            QAction* action=buttonActions.at(i);
            QToolButton* button=actionButtons.at(i);
            if (action->isVisible())
            {
                button->setToolButtonStyle(Qt::ToolButtonIconOnly);
                space-=button->sizeHint().width();
            }
        }
        if (space < 0)
        {
            // The buttons still don't fit, we need an overflow menu.
            needOverflow = true;
        }
    }

    // Calculate space available to display action buttons
    overflowMenu->clear();
    space=width() - appIcon->sizeHint().width() - viewControl->sizeHint().width();
    if (needOverflow)
    {
        space-=overflowButton->sizeHint().width();
    }

    // Show/Hide action buttons and overflow menu entries
    for (int i=0; i<buttonActions.size(); i++)
    {
        QAction* action=buttonActions.at(i);
        QToolButton* button=actionButtons.at(i);
        if (action->isVisible())
        {
            space -= button->sizeHint().width();
            if (space >= 0)
            {
                // show as button
                button->setDisabled(!buttonActions.at(i)->isEnabled());
                button->show();
            }
            else {
                // show as overflow menu entry
                button->hide();
                overflowMenu->addAction(action);
            }
        }
    }

    // Show/Hide the overflow menu button
    if (needOverflow)
    {
        overflowButton->show();
    }
    else
    {
        overflowButton->hide();
    }
}

void AndroidActionBar::addSeparator(QAction *before)
{
    viewMenu->insertSeparator(before);
}

void AndroidActionBar::addNavigation(QAction* action)
{
    QWidget::addAction(action);
    viewMenu->addAction(action);
    if (!viewMenu->isEmpty()) {
        viewControl->setMenu(viewMenu);
    }
}

void AndroidActionBar::addNavigations(QList<QAction*> actions)
{
    QWidget::addActions(actions);
    for (int i=0; i<actions.size(); i++)
    {
        addAction(actions.at(i));
    }
}

void AndroidActionBar::removeNavigation(QAction* action)
{
    QWidget::removeAction(action);
    viewMenu->removeAction(action);
    if (viewMenu->isEmpty())
    {
        viewControl->setMenu(NULL);
    }
}

void AndroidActionBar::addButton(QAction* action, QMenu *menu, int position)
{
    if (position == -1)
    {
        position = buttonActions.size();
    }
    buttonActions.insert(position,action);
    QToolButton* button = new QToolButton(this);
    button->setText(action->text());
    button->setToolTip(action->text());
    button->setIcon(action->icon());
    button->setDisabled(!action->isEnabled());
    button->setFocusPolicy(Qt::NoFocus);
    button->setAutoRaise(true);
    if(menu)
    {
        button->setMenu(menu);
        button->setPopupMode(QToolButton::InstantPopup);
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    connect(button,&QToolButton::clicked, action, &QAction::trigger);
    actionButtons.insert(position, button);
    layout->insertWidget(position+3, button);
}

void AndroidActionBar::removeButton(QAction* action)
{
    QToolButton* button = NULL;
    for (int i = 0; i < buttonActions.size(); i++)
    {

        if (buttonActions.at(i) == action)
        {
            button=actionButtons.at(i);
            break;
        }
    }
    if (button)
    {
        layout->removeWidget(button);
        actionButtons.removeOne(button);
        delete button;
        buttonActions.removeOne(action);
    }
}

void AndroidActionBar::removeButton(int position)
{
    QToolButton* button = NULL;
    for (int i = 0; i < buttonActions.size(); i++)
    {

        if (i == position)
        {
            button=actionButtons.at(i);
            break;
        }
    }
    if (button)
    {
        layout->removeWidget(button);
        actionButtons.removeOne(button);
        delete button;
        buttonActions.removeAt(position);
    }
}

void AndroidActionBar::openOverflowMenu()
{
    if (overflowButton->isVisible())
    {
        overflowButton->click();
    }
}


