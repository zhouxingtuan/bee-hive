/*
** Lua binding: liblua
** Generated automatically by tolua++-1.0.92 on Thu Sep  5 15:08:00 2019.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_liblua_open (lua_State* tolua_S);

#include "core.h"
#include "manual.hpp"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"LuaNode");
 tolua_usertype(tolua_S,"DBMysql");
 tolua_usertype(tolua_S,"Accept");
 tolua_usertype(tolua_S,"DBRedis");
 tolua_usertype(tolua_S,"MD5");
 tolua_usertype(tolua_S,"Node");
 tolua_usertype(tolua_S,"Http");
}

/* method: startTimer of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_startTimer00
static int tolua_liblua_Node_startTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  long long int timeCount = (( long long int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'startTimer'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->startTimer(callbackID,timeCount);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'startTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeTimer of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_removeTimer00
static int tolua_liblua_Node_removeTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int handle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeTimer'", NULL);
#endif
  {
   self->removeTimer(handle);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: changeTimer of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_changeTimer00
static int tolua_liblua_Node_changeTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int handle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  long long int timeCount = (( long long int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'changeTimer'", NULL);
#endif
  {
   self->changeTimer(handle,timeCount);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'changeTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTimerLeft of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_getTimerLeft00
static int tolua_liblua_Node_getTimerLeft00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int handle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTimerLeft'", NULL);
#endif
  {
   long long int tolua_ret = ( long long int)  self->getTimerLeft(handle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTimerLeft'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: broadcast of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_broadcast00
static int tolua_liblua_Node_broadcast00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int desType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int message = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int uid = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'broadcast'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->broadcast(desType,message,uid,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'broadcast'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendRequest of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_sendRequest00
static int tolua_liblua_Node_sendRequest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isstring(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int desType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int desID = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int message = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
  unsigned int uid = (( unsigned int)  tolua_tonumber(tolua_S,6,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,7,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendRequest'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendRequest(desType,desID,message,callbackID,uid,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendRequest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendResponse of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_sendResponse00
static int tolua_liblua_Node_sendResponse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isstring(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int desType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int desID = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int message = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
  unsigned int uid = (( unsigned int)  tolua_tonumber(tolua_S,6,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,7,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendResponse'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendResponse(desType,desID,message,callbackID,uid,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendResponse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseConnect of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_responseConnect00
static int tolua_liblua_Node_responseConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isstring(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,11,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int desType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int desID = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int message = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
  unsigned int uid = (( unsigned int)  tolua_tonumber(tolua_S,6,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,7,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int connType = (( unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int connHandle = (( unsigned int)  tolua_tonumber(tolua_S,10,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseConnect'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->responseConnect(desType,desID,message,callbackID,uid,ptr,length,connType,connHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseConnect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getConnectIP of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_getConnectIP00
static int tolua_liblua_Node_getConnectIP00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int connHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getConnectIP'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getConnectIP(connType,connHandle);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getConnectIP'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeConnect of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_closeConnect00
static int tolua_liblua_Node_closeConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int connHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeConnect'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->closeConnect(connType,connHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeConnect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: hash of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_hash00
static int tolua_liblua_Node_hash00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'hash'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->hash(str,length);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'hash'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: encrypt of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_encrypt00
static int tolua_liblua_Node_encrypt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  std::string result = ((std::string)  tolua_tocppstring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'encrypt'", NULL);
#endif
  {
   self->encrypt(str,length,result);
   tolua_pushcppstring(tolua_S,(const char*)result);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'encrypt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: decrypt of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_decrypt00
static int tolua_liblua_Node_decrypt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  std::string result = ((std::string)  tolua_tocppstring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'decrypt'", NULL);
#endif
  {
   self->decrypt(str,length,result);
   tolua_pushcppstring(tolua_S,(const char*)result);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'decrypt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getNodeID of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_getNodeID00
static int tolua_liblua_Node_getNodeID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getNodeID'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getNodeID();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getNodeID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setNodeID of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_setNodeID00
static int tolua_liblua_Node_setNodeID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  unsigned int nodeID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setNodeID'", NULL);
#endif
  {
   self->setNodeID(nodeID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setNodeID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getModuleType of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_getModuleType00
static int tolua_liblua_Node_getModuleType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Node",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Node* self = (const Node*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleType'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getModuleIndex of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_getModuleIndex00
static int tolua_liblua_Node_getModuleIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Node",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Node* self = (const Node*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleIndex'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleIndex();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getModuleHandle of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_getModuleHandle00
static int tolua_liblua_Node_getModuleHandle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Node",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Node* self = (const Node*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleHandle();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleHandle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: log of class  Node */
#ifndef TOLUA_DISABLE_tolua_liblua_Node_log00
static int tolua_liblua_Node_log00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Node",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Node* self = (Node*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'log'", NULL);
#endif
  {
   self->log(str);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'log'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getIP of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_getIP00
static int tolua_liblua_Accept_getIP00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Accept",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Accept* self = (const Accept*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getIP'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getIP();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getIP'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getPort of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_getPort00
static int tolua_liblua_Accept_getPort00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Accept",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Accept* self = (const Accept*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getPort'", NULL);
#endif
  {
   unsigned const short tolua_ret = ( unsigned const short)  self->getPort();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getPort'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getType of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_getType00
static int tolua_liblua_Accept_getType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Accept",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Accept* self = (const Accept*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getType'", NULL);
#endif
  {
   unsigned char tolua_ret = (unsigned char)  self->getType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getHandle of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_getHandle00
static int tolua_liblua_Accept_getHandle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Accept",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Accept* self = (const Accept*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getHandle();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getHandle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: epollRemove of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_epollRemove00
static int tolua_liblua_Accept_epollRemove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Accept",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Accept* self = (Accept*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'epollRemove'", NULL);
#endif
  {
   self->epollRemove();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'epollRemove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setBindNodeID of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_setBindNodeID00
static int tolua_liblua_Accept_setBindNodeID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Accept",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Accept* self = (Accept*)  tolua_tousertype(tolua_S,1,0);
  unsigned int nodeID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setBindNodeID'", NULL);
#endif
  {
   self->setBindNodeID(nodeID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setBindNodeID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getBindNodeID of class  Accept */
#ifndef TOLUA_DISABLE_tolua_liblua_Accept_getBindNodeID00
static int tolua_liblua_Accept_getBindNodeID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Accept",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Accept* self = (const Accept*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getBindNodeID'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getBindNodeID();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getBindNodeID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getIP of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_getIP00
static int tolua_liblua_Http_getIP00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Http",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Http* self = (const Http*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getIP'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getIP();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getIP'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getPort of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_getPort00
static int tolua_liblua_Http_getPort00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Http",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Http* self = (const Http*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getPort'", NULL);
#endif
  {
   unsigned const short tolua_ret = ( unsigned const short)  self->getPort();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getPort'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getType of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_getType00
static int tolua_liblua_Http_getType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Http",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Http* self = (const Http*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getType'", NULL);
#endif
  {
   unsigned char tolua_ret = (unsigned char)  self->getType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getHandle of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_getHandle00
static int tolua_liblua_Http_getHandle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Http",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Http* self = (const Http*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getHandle();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getHandle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: epollRemove of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_epollRemove00
static int tolua_liblua_Http_epollRemove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Http",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Http* self = (Http*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'epollRemove'", NULL);
#endif
  {
   self->epollRemove();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'epollRemove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseRequest of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_responseRequest00
static int tolua_liblua_Http_responseRequest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Http",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Http* self = (Http*)  tolua_tousertype(tolua_S,1,0);
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseRequest'", NULL);
#endif
  {
   self->responseRequest(ptr,length);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseRequest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseBegin of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_responseBegin00
static int tolua_liblua_Http_responseBegin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Http",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Http* self = (Http*)  tolua_tousertype(tolua_S,1,0);
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseBegin'", NULL);
#endif
  {
   self->responseBegin(length);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseBegin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseAppend of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_responseAppend00
static int tolua_liblua_Http_responseAppend00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Http",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Http* self = (Http*)  tolua_tousertype(tolua_S,1,0);
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseAppend'", NULL);
#endif
  {
   self->responseAppend(ptr,length);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseAppend'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseEnd of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_responseEnd00
static int tolua_liblua_Http_responseEnd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Http",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Http* self = (Http*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseEnd'", NULL);
#endif
  {
   self->responseEnd();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseEnd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseBody of class  Http */
#ifndef TOLUA_DISABLE_tolua_liblua_Http_responseBody00
static int tolua_liblua_Http_responseBody00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Http",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Http* self = (Http*)  tolua_tousertype(tolua_S,1,0);
  const char* bodyPtr = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int bodyLength = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseBody'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->responseBody(bodyPtr,bodyLength);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseBody'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: update of class  MD5 */
#ifndef TOLUA_DISABLE_tolua_liblua_MD5_update00
static int tolua_liblua_MD5_update00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MD5",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MD5* self = (MD5*)  tolua_tousertype(tolua_S,1,0);
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'update'", NULL);
#endif
  {
   self->update(ptr,length);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'update'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getString of class  MD5 */
#ifndef TOLUA_DISABLE_tolua_liblua_MD5_getString00
static int tolua_liblua_MD5_getString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MD5",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MD5* self = (MD5*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isAvailable of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_isAvailable00
static int tolua_liblua_DBMysql_isAvailable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'isAvailable'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->isAvailable();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isAvailable'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: connectMySql of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_connectMySql00
static int tolua_liblua_DBMysql_connectMySql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
  const char* ip = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* user = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* pwd = ((const char*)  tolua_tostring(tolua_S,4,0));
  const char* db = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'connectMySql'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->connectMySql(ip,user,pwd,db,port);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'connectMySql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeMysql of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_closeMysql00
static int tolua_liblua_DBMysql_closeMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeMysql'", NULL);
#endif
  {
   self->closeMysql();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pingMysql of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_pingMysql00
static int tolua_liblua_DBMysql_pingMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pingMysql'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->pingMysql();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pingMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getLastAutoID of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_getLastAutoID00
static int tolua_liblua_DBMysql_getLastAutoID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getLastAutoID'", NULL);
#endif
  {
   long long int tolua_ret = ( long long int)  self->getLastAutoID();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getLastAutoID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setAutoCommit of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_setAutoCommit00
static int tolua_liblua_DBMysql_setAutoCommit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
  bool on = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setAutoCommit'", NULL);
#endif
  {
   self->setAutoCommit(on);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setAutoCommit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: commit of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_commit00
static int tolua_liblua_DBMysql_commit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'commit'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->commit();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'commit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rollback of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_rollback00
static int tolua_liblua_DBMysql_rollback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rollback'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->rollback();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rollback'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isAvailable of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_isAvailable00
static int tolua_liblua_DBRedis_isAvailable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'isAvailable'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->isAvailable();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isAvailable'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: connectRedis of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_connectRedis00
static int tolua_liblua_DBRedis_connectRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
  const char* ip = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'connectRedis'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->connectRedis(ip,port);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'connectRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeRedis of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_closeRedis00
static int tolua_liblua_DBRedis_closeRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeRedis'", NULL);
#endif
  {
   self->closeRedis();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: identifyRedis of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_identifyRedis00
static int tolua_liblua_DBRedis_identifyRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
  const char* pwd = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'identifyRedis'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->identifyRedis(pwd);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'identifyRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getTimeStringUS */
#ifndef TOLUA_DISABLE_tolua_liblua_getTimeStringUS00
static int tolua_liblua_getTimeStringUS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   const char* tolua_ret = (const char*)  getTimeStringUS();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTimeStringUS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: writeLog */
#ifndef TOLUA_DISABLE_tolua_liblua_writeLog00
static int tolua_liblua_writeLog00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* fileName = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   writeLog(fileName,str);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'writeLog'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createRedis of class  LuaNode */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaNode_createRedis00
static int tolua_liblua_LuaNode_createRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaNode* self = (LuaNode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createRedis'", NULL);
#endif
  {
   DBRedis* tolua_ret = (DBRedis*)  self->createRedis();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DBRedis");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeRedis of class  LuaNode */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaNode_removeRedis00
static int tolua_liblua_LuaNode_removeRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DBRedis",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaNode* self = (LuaNode*)  tolua_tousertype(tolua_S,1,0);
  DBRedis* pDBRedis = ((DBRedis*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeRedis'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeRedis(pDBRedis);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createMysql of class  LuaNode */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaNode_createMysql00
static int tolua_liblua_LuaNode_createMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaNode* self = (LuaNode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createMysql'", NULL);
#endif
  {
   DBMysql* tolua_ret = (DBMysql*)  self->createMysql();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DBMysql");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeMysql of class  LuaNode */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaNode_removeMysql00
static int tolua_liblua_LuaNode_removeMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaNode* self = (LuaNode*)  tolua_tousertype(tolua_S,1,0);
  DBMysql* pDBMysql = ((DBMysql*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeMysql'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeMysql(pDBMysql);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createMD5 of class  LuaNode */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaNode_createMD500
static int tolua_liblua_LuaNode_createMD500(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaNode* self = (LuaNode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createMD5'", NULL);
#endif
  {
   MD5* tolua_ret = (MD5*)  self->createMD5();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"MD5");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createMD5'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeMD5 of class  LuaNode */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaNode_removeMD500
static int tolua_liblua_LuaNode_removeMD500(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"MD5",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaNode* self = (LuaNode*)  tolua_tousertype(tolua_S,1,0);
  MD5* pMD5 = ((MD5*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeMD5'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeMD5(pMD5);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeMD5'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_liblua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"COMMAND_CLIENT_REQUEST",COMMAND_CLIENT_REQUEST);
  tolua_constant(tolua_S,"COMMAND_HTTP_REQUEST",COMMAND_HTTP_REQUEST);
  tolua_constant(tolua_S,"COMMAND_NODE_REQUEST",COMMAND_NODE_REQUEST);
  tolua_constant(tolua_S,"COMMAND_NODE_RESPONSE",COMMAND_NODE_RESPONSE);
  tolua_constant(tolua_S,"COMMAND_NODE_ERROR",COMMAND_NODE_ERROR);
  tolua_constant(tolua_S,"COMMAND_REQUEST_TIMEOUT",COMMAND_REQUEST_TIMEOUT);
  tolua_constant(tolua_S,"COMMAND_BROADCAST",COMMAND_BROADCAST);
  tolua_cclass(tolua_S,"Node","Node","",NULL);
  tolua_beginmodule(tolua_S,"Node");
   tolua_function(tolua_S,"startTimer",tolua_liblua_Node_startTimer00);
   tolua_function(tolua_S,"removeTimer",tolua_liblua_Node_removeTimer00);
   tolua_function(tolua_S,"changeTimer",tolua_liblua_Node_changeTimer00);
   tolua_function(tolua_S,"getTimerLeft",tolua_liblua_Node_getTimerLeft00);
   tolua_function(tolua_S,"broadcast",tolua_liblua_Node_broadcast00);
   tolua_function(tolua_S,"sendRequest",tolua_liblua_Node_sendRequest00);
   tolua_function(tolua_S,"sendResponse",tolua_liblua_Node_sendResponse00);
   tolua_function(tolua_S,"responseConnect",tolua_liblua_Node_responseConnect00);
   tolua_function(tolua_S,"getConnectIP",tolua_liblua_Node_getConnectIP00);
   tolua_function(tolua_S,"closeConnect",tolua_liblua_Node_closeConnect00);
   tolua_function(tolua_S,"hash",tolua_liblua_Node_hash00);
   tolua_function(tolua_S,"encrypt",tolua_liblua_Node_encrypt00);
   tolua_function(tolua_S,"decrypt",tolua_liblua_Node_decrypt00);
   tolua_function(tolua_S,"getNodeID",tolua_liblua_Node_getNodeID00);
   tolua_function(tolua_S,"setNodeID",tolua_liblua_Node_setNodeID00);
   tolua_function(tolua_S,"getModuleType",tolua_liblua_Node_getModuleType00);
   tolua_function(tolua_S,"getModuleIndex",tolua_liblua_Node_getModuleIndex00);
   tolua_function(tolua_S,"getModuleHandle",tolua_liblua_Node_getModuleHandle00);
   tolua_function(tolua_S,"log",tolua_liblua_Node_log00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"Accept","Accept","",NULL);
  tolua_beginmodule(tolua_S,"Accept");
   tolua_function(tolua_S,"getIP",tolua_liblua_Accept_getIP00);
   tolua_function(tolua_S,"getPort",tolua_liblua_Accept_getPort00);
   tolua_function(tolua_S,"getType",tolua_liblua_Accept_getType00);
   tolua_function(tolua_S,"getHandle",tolua_liblua_Accept_getHandle00);
   tolua_function(tolua_S,"epollRemove",tolua_liblua_Accept_epollRemove00);
   tolua_function(tolua_S,"setBindNodeID",tolua_liblua_Accept_setBindNodeID00);
   tolua_function(tolua_S,"getBindNodeID",tolua_liblua_Accept_getBindNodeID00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"Http","Http","",NULL);
  tolua_beginmodule(tolua_S,"Http");
   tolua_function(tolua_S,"getIP",tolua_liblua_Http_getIP00);
   tolua_function(tolua_S,"getPort",tolua_liblua_Http_getPort00);
   tolua_function(tolua_S,"getType",tolua_liblua_Http_getType00);
   tolua_function(tolua_S,"getHandle",tolua_liblua_Http_getHandle00);
   tolua_function(tolua_S,"epollRemove",tolua_liblua_Http_epollRemove00);
   tolua_function(tolua_S,"responseRequest",tolua_liblua_Http_responseRequest00);
   tolua_function(tolua_S,"responseBegin",tolua_liblua_Http_responseBegin00);
   tolua_function(tolua_S,"responseAppend",tolua_liblua_Http_responseAppend00);
   tolua_function(tolua_S,"responseEnd",tolua_liblua_Http_responseEnd00);
   tolua_function(tolua_S,"responseBody",tolua_liblua_Http_responseBody00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"MD5","MD5","",NULL);
  tolua_beginmodule(tolua_S,"MD5");
   tolua_function(tolua_S,"update",tolua_liblua_MD5_update00);
   tolua_function(tolua_S,"getString",tolua_liblua_MD5_getString00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DBMysql","DBMysql","",NULL);
  tolua_beginmodule(tolua_S,"DBMysql");
   tolua_function(tolua_S,"isAvailable",tolua_liblua_DBMysql_isAvailable00);
   tolua_function(tolua_S,"connectMySql",tolua_liblua_DBMysql_connectMySql00);
   tolua_function(tolua_S,"closeMysql",tolua_liblua_DBMysql_closeMysql00);
   tolua_function(tolua_S,"pingMysql",tolua_liblua_DBMysql_pingMysql00);
   tolua_function(tolua_S,"getLastAutoID",tolua_liblua_DBMysql_getLastAutoID00);
   tolua_function(tolua_S,"setAutoCommit",tolua_liblua_DBMysql_setAutoCommit00);
   tolua_function(tolua_S,"commit",tolua_liblua_DBMysql_commit00);
   tolua_function(tolua_S,"rollback",tolua_liblua_DBMysql_rollback00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DBRedis","DBRedis","",NULL);
  tolua_beginmodule(tolua_S,"DBRedis");
   tolua_function(tolua_S,"isAvailable",tolua_liblua_DBRedis_isAvailable00);
   tolua_function(tolua_S,"connectRedis",tolua_liblua_DBRedis_connectRedis00);
   tolua_function(tolua_S,"closeRedis",tolua_liblua_DBRedis_closeRedis00);
   tolua_function(tolua_S,"identifyRedis",tolua_liblua_DBRedis_identifyRedis00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"getTimeStringUS",tolua_liblua_getTimeStringUS00);
  tolua_function(tolua_S,"writeLog",tolua_liblua_writeLog00);
  tolua_cclass(tolua_S,"LuaNode","LuaNode","Node",NULL);
  tolua_beginmodule(tolua_S,"LuaNode");
   tolua_function(tolua_S,"createRedis",tolua_liblua_LuaNode_createRedis00);
   tolua_function(tolua_S,"removeRedis",tolua_liblua_LuaNode_removeRedis00);
   tolua_function(tolua_S,"createMysql",tolua_liblua_LuaNode_createMysql00);
   tolua_function(tolua_S,"removeMysql",tolua_liblua_LuaNode_removeMysql00);
   tolua_function(tolua_S,"createMD5",tolua_liblua_LuaNode_createMD500);
   tolua_function(tolua_S,"removeMD5",tolua_liblua_LuaNode_removeMD500);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_liblua (lua_State* tolua_S) {
 return tolua_liblua_open(tolua_S);
};
#endif

