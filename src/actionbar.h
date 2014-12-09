#ifndef ACTIONBAR_H
#define ACTIONBAR_H

#include "menustyle.h"
#include <QWidget>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QToolButton>
#include <QProxyStyle>
#include <QStyleOption>

/**
 * Toolbar similar to Android's Action Bar, can also be used on Desktop OS.
 * The action bar shows an icon, a title and any number of action buttons.
 * Also the title can have a menu of navigation actions.
 * <p>
 * If the buttons do not fit into the window, then they are displayed
 * as an "overflow" menu on the right side.
 * <p>
 * See http://developer.android.com/design/patterns/actionbar.html
 *
 * To be used within a vertical box layout this way:
 * <pre><code>
 * MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
 *     QVBoxLayout* layout=new QVBoxLayout(this);
 *     layout->setMargin(0);
 *     layout->setSizeConstraint(QLayout::SetNoConstraint);
 *
 *     ActionBar* actionBar=new ActionBar(this);
 *     actionBar->addNavigation(new QAction("View 1",this));
 *     actionBar->addNavigation(new QAction("View 2",this));
 *     actionBar->addButton(new QAction(QIcon(":icons/search"),"Search",this));
 *     actionBar->addButton(new QAction(QIcon(":icons/share"),"Share",this));
 *     actionBar->addButton(new QAction(QIcon(":icons/settings"),"Settings",this));
 * }
 * </code></pre>
 * The option QLayout::SetNoConstraint is important on android devices to support screen rotation.
 * <p>
 * The action bar needs two icons in the resource file:
 * <ul>
 *     <li>QIcon(":icons/app") is used for the initial display.
 *     <li>QIcon(":icons/app_up") is used when you enable "up" navigation by calling setTitle().
 * </ul>
 * <p>
 * The buttons are scaled depending on the font size.
 */
class AndroidActionBar : public QWidget {
    Q_OBJECT

public:
    /**
     * Constructor, creates an Activity bar with icon and title but no action buttons.
     * The icon is loaded from the resource file via QIcon(":icons/app").
     * The title is taken from QApplication::applicationDisplayName().
     *
     * @param parent Points to the parent window.
     */
    explicit AndroidActionBar(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~AndroidActionBar();

    /**
     * Set title of the action bar.
     * @param title Either the name of the application or title of the current view within the application.
     * @param showUpButton Enables "up" navigation. Then the action bar emits signal up() on click on the icon.
     */
    void setTitle(const QString& title, bool showUpButton);

    /**
     * Adds a view navigation link to the title of the action bar.
     * @param action The action, containing at least a text and optinally an icon. The action emits signal triggered() when clicked.
     */
    void addNavigation(QAction* action);

    /**
     * Adds many view navigation links to the title of the action bar.
     * @param actions List of actions.
     * @see addAction()
     */
    void addNavigations(QList<QAction*> actions);

    /**
     * Removes a view navigation link from the title of the action bar.
     * @param action The action that had been added before.
     */
    void removeNavigation(QAction* action);

    /**
     * Adds an action button (or overflow menu item) to the action bar.
     * @param action The action, containing at least a text and optinally an icon. The action emits signal triggered() when clicked.
     * @param position Insert before this position. 0=before first button, 1=second button. Default is -1=append to the end.
     */
    void addButton(QAction* action, QMenu *menu = 0, int position=-1);

    /**
     * Removes an action button (or overflow menu item) from the action bar.
     * @param action The action that had been added before.
     */
    void removeButton(QAction* action);
    void removeButton(int position);

    /**
     * Adjust the number of buttons in the action bar. Buttons that don't fit go into the overflow menu.
     * You need to call this method after adding, removing or changing the visibility of an action.
     */
    void adjustContent();

    /**
      * Add seperators to the view menu
      */
    void addSeparator(QAction* before);

protected:

    /**
     * Overrides the paintEvent method to draw background color properly.
     */
    void paintEvent(QPaintEvent* event);

    /**
     * Overrides the resizeEvent to adjust the content depending on the new size.
     */
    void resizeEvent(QResizeEvent* event);

private:
    /** Horizontal layout of the navigation bar */
    QHBoxLayout* layout;

    /** The Button that contains the applications icon, used for "up" navigation. */
    QToolButton* appIcon;

    /** The button that contains the title, used for view navigation. */
    QToolButton* viewControl;

    /** The menu that appears when the user clicks on the title. */
    QMenu* viewMenu;

    /** List of actions for the action buttons and overflow menu. */
    QList<QAction*> buttonActions;

    /** List of action buttons, same order as buttonActions. */
    QList<QToolButton*> actionButtons;

    /** Overflow button, is only visible when there are more buttons than available space. */
    QToolButton* overflowButton;

    /** The menu that appears when the user clicks on the overflow button. */
    QMenu* overflowMenu;

    /** Used to control the size of icons in menu items. */
    MenuStyle menuStyle;

signals:
    /** Emitted when the user clicks on the icon. */
    void up();

public slots:
    /** Internally used to forward events from the appIcon button. */
    void appIconClicked();

    /** Can be used to open the overflow menu */
    void openOverflowMenu();

};

#endif // ACTIONBAR_H
