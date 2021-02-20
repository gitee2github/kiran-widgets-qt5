#ifndef KIRANTITLEBARWINDOWPRIVATE_H
#define KIRANTITLEBARWINDOWPRIVATE_H

#include <QObject>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "global_define.h"
#include "kiran-titlebar-window.h"
#include "font-monitor/font-monitor-factory.h"
#include "title-bar-layout.h"

class QGraphicsDropShadowEffect;

class KiranTitlebarWindowPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(KiranTitlebarWindow);
public:
    KiranTitlebarWindowPrivate(KiranTitlebarWindow *ptr);
    ~KiranTitlebarWindowPrivate();

private:
    void init();

    void setIcon(const QIcon &icon);
    void setTitle(const QString &title);
    void setButtonHints(KiranTitlebarWindow::TitlebarButtonHintFlags hints);
    void setWindowContentWidget(QWidget *widget);

private:
    void handlerHoverMoveEvent(QHoverEvent *ev);
    void handlerLeaveEvent();
    void handlerMouseButtonPressEvent(QMouseEvent *ev);
    void handlerMouseButtonReleaseEvent(QMouseEvent *ev);
    void handlerMouseMoveEvent(QMouseEvent *ev);
    void handlerMouseDoubleClickEvent(QMouseEvent *ev);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void initOtherWidget();
    void initShadow(bool fullScreen = false);
    void updateShadowStyle(bool active);
    Kiran::CursorPositionEnums getCursorPosition(QPoint pos);

private slots:
    void updateTitleFont(QFont font);

private:
    KiranTitlebarWindow *q_ptr;
    QLayout *m_layout;               /** < 主布局 **/

    QFrame *m_frame;                 /** < 主背景 **/
    QLayout *m_frameLayout;          /** < 主背景布局 **/

    QWidget *m_titlebarWidget;       /** < 标题栏窗口 **/
    TitlebarLayout *m_titleBarLayout;

    QLabel *m_titleIcon;             /** < 标题栏图标 **/
    QLabel *m_title;                 /** < 标题栏文本 **/

    QWidget *m_titlebarCenterWidget; /** < 标题栏中间组件 **/
    QHBoxLayout *m_customLayout;     /** < 标题栏中间组件给使用者的可自定义的控件 **/
    bool m_CustomAlignHCenter = true;

    QWidget *m_titlebarRirghtWidget; /** < 标题栏右侧组件 **/
    KiranTitlebarWindow::TitlebarButtonHintFlags m_buttonHints; /** < 右侧显示按钮枚举 **/
    QPushButton *m_btnMin;
    QPushButton *m_btnMax;
    QPushButton *m_btnClose;

    QWidget *m_windowContentWidgetWrapper;
    QWidget *m_windowContentWidget;

private:
    bool m_titlebarIsPressed;
    bool m_resizeable;
    QGraphicsDropShadowEffect *m_shadowEffect;
    bool m_isCompositingManagerRunning;

    FontMonitor *m_titleFontMonitor;
};

#endif // KIRANTITLEBARWINDOWPRIVATE_H
