// dragrectmode.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "dragrectmode.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../misc/kigpainter.h"

#include <qevent.h>
#include <qeventloop.h>

void DragRectMode::mouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e->pos(), *w );
}

void DragRectMode::leftMouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e->pos(), *w );
}

void DragRectMode::midMouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e->pos(), *w );
}

void DragRectMode::rightMouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e->pos(), *w );
}

void DragRectMode::leftReleased( QMouseEvent* e, KigWidget* w )
{
  released( e->pos(), *w );
}

void DragRectMode::midReleased( QMouseEvent* e, KigWidget* w )
{
  released( e->pos(), *w );
}

void DragRectMode::rightReleased( QMouseEvent* e, KigWidget* w )
{
  released( e->pos(), *w );
}

void DragRectMode::moved( const QPoint& p, KigWidget& w )
{
  // update the rect...
  w.updateCurPix();
  KigPainter pt( w.screenInfo(), &w.curPix );
  pt.drawFilledRect( QRect( p,  mstart ) );
  w.updateWidget( pt.overlay() );
}

void DragRectMode::released( const QPoint& p, KigWidget& w )
{
//   if (!(e->state() & (ControlButton | ShiftButton)))
//   {
//     // FIXME
//   };
  const Rect r =  w.fromScreen( QRect( mstart, p ) );
  mret = mDoc->whatIsInHere( r );
  assert( el );
  assert( el->loopLevel() == 2 );
  el->exitLoop();
}

Objects DragRectMode::run( const QPoint& start, KigMode* prev )
{
  mstart = start;

  el = new QEventLoop( 0, 0 );
  mDoc->setMode( this );
  (void) el->enterLoop();
  mDoc->setMode( prev );

  delete el;
  el = 0;

  return mret;
}

void DragRectMode::enableActions()
{
  KigMode::enableActions();
}

DragRectMode::DragRectMode( KigDocument* d )
  : KigMode( d ), el( 0 )
{

}

DragRectMode::~DragRectMode()
{
  assert( el == 0 );
}

