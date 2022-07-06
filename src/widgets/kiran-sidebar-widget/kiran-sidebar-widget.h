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
#pragma once

#include <QListWidget>

class KiranSidebarWidgetPrivate;

/**
 * @brief Kiran控制中心组件侧边栏封装
 * 基于QListWidget重新实现,提供在KiranStyle下特殊的样式绘制
 * <img src="../snapshot/kiran-sidebar-widget.png" alt="kiran-sidebar-widget.png" style="zoom:90%;" />
 *
 * 使用说明
 * - 可通过QtDesigner ui文件时,右键提升为该控件
 * - 通过CPP代码新建控件加入布局之中
 *
 * @since 2.1.0
 * @see QListWidget
 */
class KiranSidebarWidget : public QListWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(KiranSidebarWidget)
public:
    /**
     * @brief KiranSidebarWidget构造方法
     * @param parent 父控件
     */
    explicit KiranSidebarWidget(QWidget *parent = nullptr);

    /**
     * @brief KiranSidebarWidget析构方法
     */
    ~KiranSidebarWidget();

    /**
     * @brief 设置图标颜色反转
     * 当开启时，切换浅色主题时，将会翻转侧边栏图标颜色
     * 禁用时，不会进行颜色切换
     * 默认为禁用该功能
     */
    void setInvertIconPixelsEnable(bool enable);

protected:
    /* 重新实现selectionCommand，以自定义选择行为 */
    virtual QItemSelectionModel::SelectionFlags
    selectionCommand(const QModelIndex &index, const QEvent *event = Q_NULLPTR) const override;

private:
    /* KiranSidebarWidget私有类指针 */
    KiranSidebarWidgetPrivate *d_ptr;
};