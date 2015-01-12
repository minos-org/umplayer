/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2009 Ricardo Villalba <rvm@escomposlinux.org>
    Copyright (C) 2010 Ori Rejwan

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "eqslider.h"
#include <QSlider>
#include <QLabel>
#include <QPixmap>
#include <QBoxLayout>
#include <QFontMetrics>
#include "verticaltext.h"


EqSlider::EqSlider( QWidget* parent, Qt::WindowFlags f) 
	: QWidget(parent, f)
{
	setupUi(this);        
        _horLabel = new QLabel(this);
        _horLabel->resize(100, 20 /*parent->height() */ );        
        QFontMetrics fm = _horLabel->fontMetrics();
        _horLabel->setMinimumWidth(fm.width("Saturation:") + 10);
        _horLabel->setFixedHeight(20);
        _horLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        _horLabel->setMargin(0);

	_icon->setText( QString::null );
	_slider->setFocusPolicy( Qt::StrongFocus );
	_slider->setTickPosition( QSlider::TicksRight );

	_slider->setTickInterval( 10 );
	_slider->setSingleStep( 1 );
	_slider->setPageStep( 10 );
        value_label->setMinimumWidth(40);


	connect( _slider, SIGNAL(valueChanged(int)),
             this, SLOT(sliderValueChanged(int)) );
}

EqSlider::~EqSlider() {
}

/*
void EqSlider::languageChange() {
}
*/

void EqSlider::setIcon( QPixmap i) {
	_icon->setPixmap(i);
}

const QPixmap * EqSlider::icon() const {
	return _icon->pixmap();
}

void EqSlider::setLabel( QString s) {    
        _horLabel->setText(s);
        _label->setText(s) ;
}

QString EqSlider::label() const {
	return _label->text();
}

void EqSlider::setValue(int value) {
	_slider->setValue(value);
	value_label->setNum(value);
}

int EqSlider::value() const {
	return _slider->value();
}

void EqSlider::sliderValueChanged(int v) {
	emit valueChanged( v );
}

void EqSlider::setOrientation(Qt::Orientation orientation)
{
    _slider->setOrientation(orientation);
    if(orientation == Qt::Horizontal)
    {
        _slider->setTickPosition(QSlider::TicksAbove);                
        delete layout();
        _horLabel->show();
        _label->hide();
        hboxLayout = new QHBoxLayout(this);
        hboxLayout->setSpacing(10);
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout->addWidget(_icon);

        /*vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));*/
        hboxLayout->addWidget(_horLabel);
        _slider->setOrientation(Qt::Horizontal);
        hboxLayout->addWidget(_slider);

        //hboxLayout->addLayout(vboxLayout);
        hboxLayout->addWidget(value_label);
    }
    else
    {        
        _slider->setTickPosition(QSlider::TicksRight);                
        delete layout();
        _horLabel->hide();
        _label->show();        
        vboxLayout = new QVBoxLayout(this);
        vboxLayout->setSpacing(0);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->addWidget(_icon);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));        
        hboxLayout->addWidget(_label);
        _slider->setOrientation(Qt::Vertical);
        _slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        hboxLayout->addWidget(_slider);

        vboxLayout->addLayout(hboxLayout);

        vboxLayout->addWidget(value_label);
    }

}

#include "moc_eqslider.cpp"
