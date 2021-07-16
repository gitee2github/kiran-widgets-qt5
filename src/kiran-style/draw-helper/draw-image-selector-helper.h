 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
#ifndef KIRANWIDGETS_QT5_DRAW_IMAGE_SELECTOR_HELPER_H
#define KIRANWIDGETS_QT5_DRAW_IMAGE_SELECTOR_HELPER_H

class QStyleOption;
class QPainter;
class QWidget;
namespace Kiran {
    class Style;
    class StyleDetailFetcher;
    namespace DrawImageSelectorHelper{
        void drawPEKiranImageSelector(const Style *style, const QStyleOption *opt,
                                              QPainter *p,StyleDetailFetcher *fetcher,
                                              const QWidget *widget);

        void drawPEKiranImageSelectorButtonFrame(const Style *style,const QStyleOption *opt,
                                                        QPainter *p,StyleDetailFetcher *fetcher,
                                                        const QWidget *widget);

        void drawPEKiranImageSelectorPrevButtonArrow(const Style *style,const QStyleOption *opt,
                                                     QPainter *p,StyleDetailFetcher *fetcher,
                                                     const QWidget *widget);

        void drawPEKiranImageSelectorNextButtonArrow(const Style *style,const QStyleOption *opt,
                                                     QPainter *p,StyleDetailFetcher *fetcher,
                                                     const QWidget *widget);

        void drawPEKiranImageSelectorItem(const Style *style,const QStyleOption *opt,
                                          QPainter *p,StyleDetailFetcher *fetcher,
                                          const QWidget *widget);
    }
}


#endif //KIRANWIDGETS_QT5_DRAW_IMAGE_SELECTOR_HELPER_H
