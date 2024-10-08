// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "vector_type.h"

#include "bogus_imp.h"
#include "other_imp.h"
#include "point_imp.h"

static const ArgsParser::spec argsspecVector[] = {
    {PointImp::stype(), kli18n("Construct a vector from this point"), kli18n("Select the start point of the new vector..."), true},
    {PointImp::stype(), kli18n("Construct a vector to this point"), kli18n("Select the end point of the new vector..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(VectorType)

VectorType::VectorType()
    : ObjectABType("Vector", argsspecVector, 2)
{
}

VectorType::~VectorType()
{
}

const VectorType *VectorType::instance()
{
    static const VectorType t;
    return &t;
}

ObjectImp *VectorType::calcx(const Coordinate &a, const Coordinate &b) const
{
    return new VectorImp(a, b);
}

const ObjectImpType *VectorType::resultId() const
{
    return VectorImp::stype();
}

static const ArgsParser::spec argsspecVectorSum[] = {
    {VectorImp::stype(),
     kli18n("Construct the vector sum of this vector and another one."),
     kli18n("Select the first of the two vectors of which you want to construct the sum..."),
     false},
    {VectorImp::stype(),
     kli18n("Construct the vector sum of this vector and the other one."),
     kli18n("Select the other of the two vectors of which you want to construct the sum..."),
     false},
    {PointImp::stype(), kli18n("Construct the vector sum starting at this point."), kli18n("Select the point to construct the sum vector in..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(VectorSumType)

VectorSumType::VectorSumType()
    : ArgsParserObjectType("VectorSum", argsspecVectorSum, 3)
{
}

VectorSumType::~VectorSumType()
{
}

const VectorSumType *VectorSumType::instance()
{
    static const VectorSumType t;
    return &t;
}

ObjectImp *VectorSumType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    const VectorImp &a = *static_cast<const VectorImp *>(args[0]);
    const VectorImp &b = *static_cast<const VectorImp *>(args[1]);
    const PointImp &p = *static_cast<const PointImp *>(args[2]);

    return new VectorImp(p.coordinate(), p.coordinate() + a.dir() + b.dir());
}

const ObjectImpType *VectorSumType::resultId() const
{
    return VectorImp::stype();
}
