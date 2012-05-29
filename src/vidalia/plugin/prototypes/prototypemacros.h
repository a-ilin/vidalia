#define GET_AND_CALL(type, func, res) \
  type obj = qscriptvalue_cast<type>(thisObject()); \
  if(obj) \
    res = obj->func;

#define MERGE2(result, errmsg) \
  QVariant(QList<QVariant>() << result << errmsg);

#define DEF_TYPE0(type, retType, func, call) \
retType \
type##Prototype::func \
{ \
  type *obj = qscriptvalue_cast<type *>(thisObject()); \
  if(obj) \
    return obj->call; \
}

#define DEF_TYPE1(type, resType, func, call) \
QVariant \
type##Prototype::func \
{ \
  resType res; \
  QString errmsg; \
\
  type *obj = qscriptvalue_cast<type *>(thisObject()); \
  if(obj) \
    res = obj->call; \
  QList<QVariant> vals; \
  vals << res << QVariant(errmsg); \
\
  return vals; \
}

#define DEF_TYPE2(type, resType, ansType, func, call) \
QVariant \
type##Prototype::func \
{ \
  resType res; \
  ansType ans; \
  QString errmsg; \
\
  type *obj = qscriptvalue_cast<type *>(thisObject()); \
  if(obj) \
    res = obj->call; \
  QList<QVariant> vals; \
  vals << QVariant(ans) << res << QVariant(errmsg); \
\
  return vals; \
}
