/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiranwidgets-qt5 is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "kiran-titlebar-window-private.h"
#include "../../public/xlib-helper.h"
#include "global_define.h"
#include "title-bar-layout.h"

#include <xcb/xcb.h>
#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QEvent>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QScreen>
#include <QStyle>
#include <QTimer>
#include <QWindow>

using namespace Kiran;

static const QColor inactivatedColor("#222222");
static const int    inactivatedBlurRadius = 10;

static const QColor activatedColor("#000000");
static const int    activatedBlurRadius = 18;

#define XCB_GENERIC_EVENT_TYPE "xcb_generic_event_t"

KiranTitlebarWindowPrivate::KiranTitlebarWindowPrivate(KiranTitlebarWindow *ptr)
    : q_ptr(ptr),
      m_layout(nullptr),
      m_frame(nullptr),
      m_titlebarWidget(nullptr),
      m_titleIcon(nullptr),
      m_title(nullptr),
      m_customLayout(nullptr),
      m_buttonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints),
      m_btnMin(nullptr),
      m_btnMax(nullptr),
      m_btnClose(nullptr),
      m_windowContentWidgetWrapper(nullptr),
      m_windowContentWidget(nullptr),
      m_titlebarIsPressed(false),
      m_resizeable(true),
      m_shadowEffect(nullptr),
      m_isCompositingManagerRunning(false)
{
    m_isCompositingManagerRunning = QX11Info::isCompositingManagerRunning();

    m_shadowEffect = new QGraphicsDropShadowEffect(q_ptr);
    m_shadowEffect->setOffset(0, 0);
    q_func()->setGraphicsEffect(m_shadowEffect);
}

KiranTitlebarWindowPrivate::~KiranTitlebarWindowPrivate()
{
    delete m_titleFontMonitor;
}

void KiranTitlebarWindowPrivate::init()
{
    initOtherWidget();

    /// 标题栏字体监控
    m_titleFontMonitor = FontMonitorFactory::createAppTitleFontMonitor();
    if (m_titleFontMonitor != nullptr)
    {
        updateTitleFont(QFont());
        connect(m_titleFontMonitor, &FontMonitor::fontChanged, this, &KiranTitlebarWindowPrivate::updateTitleFont);
    }
    /// 内容栏
    auto contentWidget = new QWidget;
    setWindowContentWidget(contentWidget);
    /// 加载样式表
    QFile file(DEFAULT_THEME_PATH);
    if (file.open(QIODevice::ReadOnly))
    {
        QString titlebarStyle = file.readAll();
        q_func()->setStyleSheet(q_func()->styleSheet() + titlebarStyle);
    }
    /// 处理窗口事件
    q_ptr->installEventFilter(this);
}

void KiranTitlebarWindowPrivate::setIcon(const QIcon &icon)
{
    m_titleIcon->setPixmap(icon.pixmap(16, 16));
}

void KiranTitlebarWindowPrivate::setTitle(const QString &title)
{
    m_title->setText(title);
}

void KiranTitlebarWindowPrivate::setButtonHints(KiranTitlebarWindow::TitlebarButtonHintFlags hints)
{
    m_buttonHints = hints;
    m_btnMax->setVisible((m_buttonHints & KiranTitlebarWindow::TitlebarMaximizeButtonHint));
    m_btnMin->setVisible((m_buttonHints & KiranTitlebarWindow::TitlebarMinimizeButtonHint));
    m_btnClose->setVisible((m_buttonHints & KiranTitlebarWindow::TitlebarCloseButtonHint));
}

void KiranTitlebarWindowPrivate::setWindowContentWidget(QWidget *widget)
{
    if (m_windowContentWidget != nullptr)
    {
        delete m_windowContentWidget;
        m_windowContentWidget = nullptr;
    }
    m_windowContentWidget = widget;
    m_windowContentWidget->setParent(m_windowContentWidgetWrapper);
    m_windowContentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_windowContentWidgetWrapper->layout()->addWidget(m_windowContentWidget);
}

void KiranTitlebarWindowPrivate::handlerHoverMoveEvent(QHoverEvent *ev)
{
    if (!m_resizeable)
    {
        return;
    }
    CursorPositionEnums postion = getCursorPosition(QCursor::pos());
    if (postion == CursorPosition_None || q_ptr->isMaximized())
    {
        q_func()->unsetCursor();
        return;
    }
    switch (postion)
    {
    case CursorPosition_LeftTop:
    case CursorPosition_RightBottom:
        q_func()->setCursor(Qt::SizeFDiagCursor);
        break;
    case CursorPosition_RightTop:
    case CursorPosition_LeftBottom:
        q_func()->setCursor(Qt::SizeBDiagCursor);
        break;
    case CursorPosition_Left:
    case CursorPosition_Right:
        q_func()->setCursor(Qt::SizeHorCursor);
        break;
    case CursorPosition_Top:
    case CursorPosition_Bottom:
        q_func()->setCursor(Qt::SizeVerCursor);
        break;
    default:
        break;
    }
}

void KiranTitlebarWindowPrivate::handlerLeaveEvent()
{
    q_func()->unsetCursor();
}

void KiranTitlebarWindowPrivate::handlerMouseButtonPressEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton)
    {
        return;
    }

    if (m_resizeable && !q_func()->isMaximized())
    {
        CursorPositionEnums postion = getCursorPosition(ev->globalPos());
        if (postion != CursorPosition_None)
        {
            QPoint pos = QCursor::pos();
            pos *= q_func()->devicePixelRatio();
            XLibHelper::sendResizeEvent(QX11Info::display(),
                                        postion, q_ptr->winId(),
                                        pos.x(),
                                        pos.y());
            return;
        }
    }

    if (m_titlebarWidget->frameGeometry().contains(ev->pos()))
    {
        m_titlebarIsPressed = true;
    }
}

void KiranTitlebarWindowPrivate::handlerMouseButtonReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        if (m_titlebarIsPressed)
        {
            m_titlebarIsPressed = false;
        }
    }
}

#include <QApplication>
void KiranTitlebarWindowPrivate::handlerMouseMoveEvent(QMouseEvent *ev)
{
    ///判断是否点击标题栏区域
    if (m_titlebarIsPressed)
    {
        QPoint pos = QCursor::pos();
        pos *= q_func()->devicePixelRatio();
        XLibHelper::sendWMMoveEvent(QX11Info::display(),
                                    q_func()->winId(),
                                    pos.x(),
                                    pos.y());
        ///NOTE:在此之后获取不到MouseRelease事件,需复位按钮按压
        m_titlebarIsPressed = false;
        return;
    }
}

void KiranTitlebarWindowPrivate::handlerMouseDoubleClickEvent(QMouseEvent *ev)
{
    if ((ev->button() != Qt::LeftButton) || !m_resizeable)
    {
        return;
    }

    if (m_titlebarWidget->frameGeometry().contains(ev->pos()))
    {
        if (q_func()->isMaximized())
        {
            q_func()->showNormal();
        }
        else
        {
            q_func()->showMaximized();
        }
    }
}

void KiranTitlebarWindowPrivate::initOtherWidget()
{
    ///主布局
    m_layout = new QVBoxLayout(q_ptr);
    m_layout->setObjectName("KiranTitlebarMainLayout");
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    ///背景
    m_frame = new QFrame(q_ptr);
    m_frame->setAttribute(Qt::WA_Hover);
    m_layout->addWidget(m_frame);
    m_frame->setObjectName("KiranTitlebarFrame");
    m_frameLayout = new QVBoxLayout(m_frame);
    m_frameLayout->setMargin(0);
    m_frameLayout->setSpacing(0);

    ///标题栏
    m_titlebarWidget = new QWidget(m_frame);
    m_titlebarWidget->setFocusPolicy(Qt::NoFocus);
    m_titlebarWidget->setObjectName("KiranTitlebarWidget");
    m_titlebarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_titlebarWidget->setFixedHeight(60);
    m_frameLayout->addWidget(m_titlebarWidget);
    m_titleBarLayout = new TitlebarLayout(m_titlebarWidget);
    m_titleBarLayout->setMargin(0);
    m_titleBarLayout->setSpacing(0);
    m_titleBarLayout->setObjectName("KiranTitlebarLayout");

    ///标题栏居左部分
    //标题栏图标
    m_titleIcon = new QLabel(m_titlebarWidget);
    m_titleIcon->setObjectName("KiranTitlebarIcon");
    m_titleIcon->setFixedSize(24, 24);
    m_titleBarLayout->setTitleBarIconLabel(m_titleIcon);
    m_titleBarLayout->setTitleBarIconMargin(QMargins(20, 0, 12, 0));

    //标题
    m_title = new QLabel(m_titlebarWidget);
    m_title->setObjectName("KiranTitlebarTitle");
    m_title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_title->installEventFilter(this);
    m_titleBarLayout->setTitleBarTitleLabel(m_title);

    ///标题栏居中部分
    //自定义控件区域
    m_titlebarCenterWidget = new QWidget(m_titlebarWidget);
    m_titlebarCenterWidget->setObjectName("KiranTitlebarCenterWidget");
    m_titlebarCenterWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_titleBarLayout->setTitleBarCustomWidget(m_titlebarCenterWidget);
    m_customLayout = new QHBoxLayout(m_titlebarCenterWidget);
    m_customLayout->setMargin(0);
    m_customLayout->setSpacing(0);
    m_customLayout->setObjectName("KiranTitlebarCustomLayout");

    ///标题栏居右部分
    m_titlebarRirghtWidget = new QWidget(m_titlebarWidget);
    m_titlebarRirghtWidget->setObjectName("KiranTitlebarRightWidget");
    m_titlebarRirghtWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_titleBarLayout->setTitleBarRightWidget(m_titlebarRirghtWidget);
    m_titleBarLayout->setTitleBarRightWidgetMargin(QMargins(0, 0, 0, 24));
    QHBoxLayout *titlebarRightlayout = new QHBoxLayout(m_titlebarRirghtWidget);
    titlebarRightlayout->setSpacing(10);

    //占位
    QSpacerItem *spacerItem = new QSpacerItem(0, 20, QSizePolicy::Expanding);
    titlebarRightlayout->addItem(spacerItem);

    //最小化
    m_btnMin = new QPushButton(m_titlebarWidget);
    m_btnMin->setObjectName("KiranTitlebarMinButton");
    m_btnMin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_btnMin->setVisible(false);
    m_btnMin->setFocusPolicy(Qt::NoFocus);
    connect(m_btnMin,&QPushButton::clicked,[this](bool checked){
        Q_UNUSED(checked);
        q_ptr->showMinimized();
    });
    titlebarRightlayout->addWidget(m_btnMin, 0, Qt::AlignVCenter);

    //最大化
    m_btnMax = new QPushButton(m_titlebarWidget);
    m_btnMax->setObjectName("KiranTitlebarMaxButton");
    m_btnMax->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_btnMax->setVisible(false);
    m_btnMax->setFocusPolicy(Qt::NoFocus);
    connect(m_btnMax, &QPushButton::clicked, [this](bool checked) {
        Q_UNUSED(checked);
        if (q_ptr->isMaximized())
        {
            q_ptr->showNormal();
        }
        else
        {
            q_ptr->showMaximized();
        }
    });
    titlebarRightlayout->addWidget(m_btnMax, 0, Qt::AlignVCenter);

    //关闭
    m_btnClose = new QPushButton(m_titlebarWidget);
    m_btnClose->setObjectName("KiranTitlebarCloseButton");
    m_btnClose->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_btnClose->setVisible(false);
    m_btnClose->setFocusPolicy(Qt::NoFocus );
    connect(m_btnClose, &QPushButton::clicked, [this](bool checked) {
        Q_UNUSED(checked);
        q_ptr->close();
    });
    titlebarRightlayout->addWidget(m_btnClose, 0, Qt::AlignVCenter);

    setButtonHints(m_buttonHints);

    ///内容窗口包装
    m_windowContentWidgetWrapper = new QWidget(m_frame);
    m_windowContentWidgetWrapper->setObjectName("KiranTitlebarContentWrapper");
    m_frameLayout->addWidget(m_windowContentWidgetWrapper);
    m_windowContentWidgetWrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout *windowContentWidgetWrapperLayout = new QHBoxLayout(m_windowContentWidgetWrapper);
    windowContentWidgetWrapperLayout->setSpacing(0);
    windowContentWidgetWrapperLayout->setMargin(0);
}

void KiranTitlebarWindowPrivate::initShadow()
{
    bool showShadow = m_isCompositingManagerRunning;
    if (Q_LIKELY(m_shadowEffect))
    {
        m_shadowEffect->setEnabled(showShadow);
    }
    if (Q_LIKELY(m_layout))
    {
        m_layout->setMargin(showShadow ? SHADOW_BORDER_WIDTH : 0);
    }
    int gtkFrameExtent = showShadow ? SHADOW_BORDER_WIDTH : 0;
    XLibHelper::SetShadowWidth(QX11Info::display(),
                               q_ptr->winId(),
                               gtkFrameExtent,
                               gtkFrameExtent,
                               gtkFrameExtent,
                               gtkFrameExtent);
}

void KiranTitlebarWindowPrivate::updateShadowStyle(bool active)
{
    bool showShadow = m_isCompositingManagerRunning && (!(q_func()->windowState() & Qt::WindowFullScreen));
    if (Q_LIKELY(m_shadowEffect && showShadow))
    {
        m_shadowEffect->setColor(active ? activatedColor : inactivatedColor);
        m_shadowEffect->setBlurRadius(active ? activatedBlurRadius : inactivatedBlurRadius);
    }
}

CursorPositionEnums KiranTitlebarWindowPrivate::getCursorPosition(QPoint pos)
{
    QPoint     frameLeftTop = m_frame->mapToGlobal(QPoint(0, 0));
    int        frameX = frameLeftTop.x(), frameY = frameLeftTop.y();
    int        frameWidth = m_frame->width(), frameHeight = m_frame->height();
    static int borderWidth = 5;

    QRect topBorderRect(frameX, frameY, frameWidth, borderWidth);
    QRect bottomBorderRect(frameX, frameY + frameHeight - borderWidth, frameWidth, borderWidth);
    QRect leftBorderRect(frameX, frameY, borderWidth, frameHeight);
    QRect rightBorderRect(frameX + frameWidth - borderWidth, frameY, borderWidth, frameHeight);

    CursorPositionEnums positions = CursorPosition_None;
    if (topBorderRect.contains(pos))
    {
        positions |= CursorPosition_Top;
    }
    if (bottomBorderRect.contains(pos))
    {
        positions |= CursorPosition_Bottom;
    }
    if (leftBorderRect.contains(pos))
    {
        positions |= CursorPosition_Left;
    }
    if (rightBorderRect.contains(pos))
    {
        positions |= CursorPosition_Right;
    }

    return positions;
}

void KiranTitlebarWindowPrivate::updateTitleFont(QFont font)
{
    if (m_titleFontMonitor != nullptr)
    {
        qDebug() << "update title font" << m_titleFontMonitor->currentFont();
        m_title->setFont(m_titleFontMonitor->currentFont());
    }
}

bool KiranTitlebarWindowPrivate::eventFilter(QObject *obj, QEvent *event)
{
    //NOTE:用户标题栏暂时需要使用窗口管理器单独设置的字体，不和程序字体通用
    if (obj == m_title && event->type() == QEvent::ApplicationFontChange)
    {
        return true;
    }
    // 对窗口事件进行处理
    if (obj == q_ptr)
    {
        switch (event->type())
        {
        case QEvent::HoverMove:
            handlerHoverMoveEvent(dynamic_cast<QHoverEvent *>(event));
            break;
        case QEvent::Leave:
            handlerLeaveEvent();
            break;
        case QEvent::MouseButtonPress:
            handlerMouseButtonPressEvent(dynamic_cast<QMouseEvent *>(event));
            break;
        case QEvent::MouseButtonRelease:
            handlerMouseButtonReleaseEvent(dynamic_cast<QMouseEvent *>(event));
            break;
        case QEvent::MouseMove:
            handlerMouseMoveEvent(dynamic_cast<QMouseEvent *>(event));
            break;
        case QEvent::ShowToParent:
        {
            initShadow();
            break;
        }
        case QEvent::MouseButtonDblClick:
            handlerMouseDoubleClickEvent(dynamic_cast<QMouseEvent *>(event));
            break;
        case QEvent::WindowStateChange:
            //窗口状态变更时，加载不同的样式
            QTimer::singleShot(0, [this]() {
                q_ptr->style()->polish(m_frame);
            });
            break;
        case QEvent::ActivationChange:
            updateShadowStyle(q_ptr->isActiveWindow());
            break;
        default:
            break;
        }
    }
    return QObject::eventFilter(obj, event);
}