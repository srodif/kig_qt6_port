// nobject.cc
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

#include "object.h"

#include "object_type.h"
#include "object_imp.h"
#include "curve_imp.h"
#include "../misc/kigpainter.h"

#include <qpen.h>
#include <functional>
#include <algorithm>

RealObject::RealObject( const ObjectType* type, const Objects& parents )
  : ObjectWithParents( parents ),
    mcolor( Qt::blue ), mselected( false ), mshown( true ),
    mwidth( -1 ), mtype( type ), mimp( 0 )
{
}

RealObject::~RealObject()
{
  delete mimp;
}

void RealObject::draw( KigPainter& p, bool ss ) const
{
  if ( ! mshown ) return;
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( mselected && ss ? Qt::red : mcolor );
  p.setPen( QPen ( mselected && ss ? Qt::red : mcolor,  1) );
  p.setWidth( mwidth );
  assert( mimp );
  mimp->draw( p );
}

bool RealObject::contains( const Coordinate& p, const ScreenInfo& si ) const
{
  return mimp->contains( p, mwidth, si );
}

bool RealObject::inRect( const Rect& r ) const
{
  return mimp->inRect( r );
}

void RealObject::move( const Coordinate& from, const Coordinate& dist )
{
  mtype->move( this, from, dist );
}

void ObjectWithParents::calc()
{
  using namespace std;
  Args a;
  a.reserve( mparents.size() );
  transform( mparents.begin(), mparents.end(),
             back_inserter( a ), mem_fun( &Object::imp ) );
  calc( a );
}

void RealObject::reset( const ObjectType* t, const Objects& parents )
{
  setType( t );
  setParents( parents );
}

void RealObject::setImp( ObjectImp* i )
{
  delete mimp;
  mimp = i;
}

bool RealObject::canMove() const
{
  return mtype->canMove();
}

Object::~Object()
{
  // tell our children that we're dead, so they don't try to tell us
  // that they're dying too, which would cause segfaults...
  for ( uint i = 0; i < mchildren.size(); ++i )
    mchildren[i]->delParent( this );
}

bool Object::hasimp( int type ) const
{
  assert( imp() );
  return imp()->inherits( type );
}

const uint Object::numberOfProperties() const
{
  return imp()->numberOfProperties();
}

ObjectImp* Object::property( uint which, const KigWidget& w ) const
{
  return imp()->property( which, w );
}

const QCStringList Object::properties() const
{
  return imp()->properties();
}

const Objects& Object::children() const
{
  return mchildren;
}

const Objects Object::getAllChildren() const
{
  Objects ret;
  // objects to iterate over...
  Objects objs = mchildren;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while( !objs.empty() )
  {
    for( Objects::iterator i = objs.begin();
         i != objs.end(); ++i )
    {
      ret.upush( *i );
      objs2 |= (*i)->children();
    };
    objs = objs2;
    objs2.clear();
  };
  return ret;
}

void Object::addChild( Object* o )
{
  if ( ! mchildren.contains( o ) )
  {
    mchildren.push_back( o );
    childAdded();
  };
}

void Object::delChild( Object* o )
{
  if ( mchildren.contains( o ) )
  {
    mchildren.remove( o );
    childRemoved();
  };
}

void ObjectWithParents::addParent( Object* o )
{
  mparents.push_back( o );
  o->addChild( this );
}

void ObjectWithParents::delParent( Object* o )
{
  mparents.remove( o );
}

void ObjectWithParents::setParents( const Objects& parents )
{
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->delChild( this );
  mparents = parents;
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->addChild( this );
}

Objects ObjectWithParents::parents() const
{
  return mparents;
}

ObjectWithParents::~ObjectWithParents()
{
  // tell our parents that we're dead...
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->delChild( this );
}

const ObjectImp* RealObject::imp() const
{
  return mimp;
}

const ObjectType* RealObject::type() const
{
  return mtype;
}

void RealObject::calc( const Args& a )
{
  delete mimp;
  mimp = mtype->calc( a );
}

Object::Object()
{
}

ObjectWithParents::ObjectWithParents( const Objects& parents )
  : mparents( parents )
{
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->addChild( this );
}

DataObject::DataObject( ObjectImp* imp )
  : mimp( imp ), mrefs( 0 )
{
}

DataObject::~DataObject()
{
  delete mimp;
}

const ObjectImp* DataObject::imp() const
{
  return mimp;
}

Objects DataObject::parents() const
{
  return Objects();
}

void DataObject::draw( KigPainter&, bool ) const
{
}

bool DataObject::contains( const Coordinate&, const ScreenInfo& ) const
{
  return false;
}

bool DataObject::inRect( const Rect& ) const
{
  return false;
}

bool DataObject::canMove() const
{
  return false;
}

void DataObject::move( const Coordinate&, const Coordinate& )
{
//    assert( false );
}

void DataObject::calc()
{
}

void DataObject::childAdded()
{
  ++mrefs;
}

void DataObject::childRemoved()
{
  if ( --mrefs <= 0 ) delete this;
}

void Object::childRemoved()
{
}

void Object::childAdded()
{
}

void Object::addParent( Object* )
{
  assert( false );
}

void Object::delParent( Object* )
{
  assert( false );
}

void Object::setParents( const Objects& )
{
  assert( false );
}

bool Object::inherits( int ) const
{
  return false;
}

bool RealObject::inherits( int type ) const
{
  return type == ID_RealObject;
}

void RealObject::calc()
{
  // no idea why this is necessary
  ObjectWithParents::calc();
}

void DataObject::setImp( ObjectImp* imp )
{
  delete mimp;
  mimp = imp;
}

bool DataObject::inherits( int type ) const
{
  return type == ID_DataObject;
}

bool Object::valid() const
{
  return imp()->valid();
}

bool RealObject::shown() const
{
  return mshown;
}

bool DataObject::shown() const
{
  return false;
}

void RealObject::setType( const ObjectType* t )
{
  mtype = t;
}

void RealObject::setSelected( bool s )
{
  mselected = s;
}

void Object::setSelected( bool )
{
}

void Object::setShown( bool )
{
}

void RealObject::setShown( bool shown )
{
  mshown = shown;
}
