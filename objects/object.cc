// object.cc
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
#include "../kig/kig_part.h"

#include <qpen.h>
#include <assert.h>
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

bool RealObject::contains( const Coordinate& p, const KigWidget& w ) const
{
  return mimp->contains( p, mwidth, w );
}

bool RealObject::inRect( const Rect& r, const KigWidget& w ) const
{
  return mimp->inRect( r, mwidth, w );
}

void RealObject::move( const Coordinate& from, const Coordinate& dist,
                       const KigDocument& d )
{
  mtype->move( this, from, dist, d );
}

void ObjectWithParents::calc( const KigDocument& d )
{
  using namespace std;
  Args a;
  a.reserve( mparents.size() );
  transform( mparents.begin(), mparents.end(),
             back_inserter( a ), mem_fun( &Object::imp ) );
  calc( a, d );
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

ObjectImp* Object::property( uint which, const KigDocument& w ) const
{
  return imp()->property( which, w );
}

const QCStringList Object::propertiesInternalNames() const
{
  return imp()->propertiesInternalNames();
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
  mchildren.upush( o );
}

void Object::delChild( Object* o )
{
  mchildren.remove( o );
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

void ObjectWithParents::setParents( const Objects& parents, KigDocument& doc )
{
  for ( uint i = 0; i < mparents.size(); ++i )
  {
    mparents[i]->delChild( this );
    if ( mparents[i]->isInternal() && mparents[i]->children().empty() &&
         ! parents.contains( mparents[i] ) )
    {
      // mparents[i] is an internal object that is no longer used..
      // so we remove it from the document, and delete it..
      doc._delObject( mparents[i] );
      delete mparents[i];
    };
  };
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

void RealObject::calc( const Args& a, const KigDocument& d )
{
  delete mimp;
  mimp = mtype->calc( a, d );
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
  : mimp( imp )
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

bool DataObject::contains( const Coordinate&, const KigWidget& ) const
{
  return false;
}

bool DataObject::inRect( const Rect&, const KigWidget& ) const
{
  return false;
}

void DataObject::calc( const KigDocument& )
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

void Object::setParents( const Objects&, KigDocument& )
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

void RealObject::calc( const KigDocument& d )
{
  // no idea why this is necessary
  ObjectWithParents::calc( d );
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

bool RealObject::isInternal() const
{
  return false;
}

bool DataObject::isInternal() const
{
  return true;
}

PropertyObject::PropertyObject( Object* parent, int id )
  : Object(), mimp( 0 ), mparent( parent ), mpropid( id )
{
  mparent->addChild( this );
}

PropertyObject::~PropertyObject()
{
  if ( mparent ) mparent->delChild( this );
  delete mimp;
}

bool PropertyObject::inherits( int type ) const
{
  return type == ID_PropertyObject;
}

bool PropertyObject::isInternal() const
{
  return true;
}

const ObjectImp* PropertyObject::imp() const
{
  return mimp;
}

Objects PropertyObject::parents() const
{
  return Objects( mparent );
}

void PropertyObject::draw( KigPainter&, bool ) const
{
}

bool PropertyObject::contains( const Coordinate&, const KigWidget& ) const
{
  return false;
}

bool PropertyObject::inRect( const Rect&, const KigWidget& ) const
{
  return false;
}

void PropertyObject::calc( const KigDocument& d )
{
  delete mimp;
  mimp = mparent->property( mpropid, d );
}

bool PropertyObject::shown() const
{
  return false;
}

const Object* PropertyObject::parent() const
{
  return mparent;
}

int PropertyObject::propId() const
{
  return mpropid;
}

void PropertyObject::delParent( Object* o )
{
  // if a parent gets deleted before us, we set mparent to zero, and
  // wait to be deleted ourselves ( which will be immediately after
  // this )
  if ( mparent == o ) mparent = 0;
}

void Object::setColor( const QColor& )
{
}

void RealObject::setColor( const QColor& c )
{
  mcolor = c;
}

bool Object::canMove() const
{
  return false;
}

void Object::move( const Coordinate&, const Coordinate&,
                   const KigDocument& )
{
  assert( false );
}

QColor Object::color() const
{
  assert( false );
}

QColor RealObject::color() const
{
  return mcolor;
}

void RealObject::setWidth( int width )
{
  mwidth = width;
}


