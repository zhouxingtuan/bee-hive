#ifndef __LUA_RAPIDJSON_TOLUAHANDLER_HPP__
#define __LUA_RAPIDJSON_TOLUAHANDLER_HPP__

#include <vector>
#include <lua.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include "luax.hpp"

namespace values {
	typedef rapidjson::Document::AllocatorType Allocator;

	int json_null(lua_State* L);
	int json_null_nil(lua_State* L);
	int json_null_string(lua_State* L);

	inline bool isnull(lua_State* L, int idx)
	{
		lua_pushvalue(L, idx); // [value]

		json_null(L); // [value, json.null]
		auto is = lua_rawequal(L, -1, -2) != 0;
		lua_pop(L, 2); // []

		return is;
	}

	inline bool hasJsonType(lua_State* L, int idx, bool& isarray)
	{
		auto has = false;
		if (lua_getmetatable(L, idx)) {
			// [metatable]
			lua_getfield(L, -1, "__jsontype"); // [metatable, metatable.__jsontype]
			if (lua_isstring(L, -1))
			{
				size_t len;
				auto s = lua_tolstring(L, -1, &len);
				isarray = strncmp(s, "array", 6) == 0;
				has = true;
			}
			lua_pop(L, 2); // []
		}

		return has;
	}

	inline bool isarray(lua_State* L, int idx, size_t* pMax) {
		auto arr = false;
//		if (hasJsonType(L, idx, arr)) // any table with a meta field __jsontype set to 'array' are arrays
//		{
//			if(pMax && arr){
//				*pMax = luax::rawlen(L, idx); // any table has length > 0 are treat as array.
//			}
//			return arr;
//		}

		size_t max = 0;
		size_t count = 0;
		lua_Number key;
		// [table]
		int nIndex = lua_gettop(L);
		lua_pushnil(L);						// [table, nil]
//		while (lua_next(L, -2))
		while (lua_next(L, nIndex))
		{
			// [table, key, value]
			if (lua_type(L, -2) != LUA_TNUMBER)
			{
				// pop key and value
//				lua_pop(L, 2); // []
                lua_settop(L, nIndex);
				if(count > 0){
					fprintf(stderr, "rapidjson encode number keys detect when check array. will be skip\n");
				}
				return false;
			}
			// pop value, leaving original key
			lua_pop(L, 1);
			// [table, key]
			key = (lua_Number)lua_tonumber(L, -1);
			if((lua_Number)floor(key) == (lua_Number)key && key > 0){
				++count;
				if((size_t)key > max){
					max = (size_t)key;
				}
				continue;
			}
			fprintf(stderr, "rapidjson encode get bad array key=%lf\n", key);
			// [table]
//			lua_pop(L, 1); // []
            lua_settop(L, nIndex);
			return false;
		}
		// [table]
//		lua_pop(L, 1); // []
        lua_settop(L, nIndex);

		// 如果长度比例不对，不判定为array
		if( max - count > max / 2 ){
			fprintf(stderr, "rapidjson encode get bad array (max - count > max / 2) max=%d count=%d\n", max, count);
			return false;
		}


		if(pMax){
			*pMax = max;
		}
		return true;
//		return luax::rawlen(L, idx) > 0; // any table has length > 0 are treat as array.
	}



	/**
	 * Handle json SAX events and create Lua object.
	 */
	struct ToLuaHandler {
		explicit ToLuaHandler(lua_State* aL) : L(aL) { stack_.reserve(32); }

		bool Null() {
//			json_null_nil(L);
			lua_pushnil(L);
			context_.submit(L);
			return true;
		}
		bool Bool(bool b) {
			lua_pushboolean(L, b);
			context_.submit(L);
			return true;
		}
		bool Int(int i) {
			lua_pushinteger(L, i);
			context_.submit(L);
			return true;
		}
		bool Uint(unsigned u) {
			if (sizeof(lua_Integer) > sizeof(unsigned int) || u <= static_cast<unsigned>(std::numeric_limits<lua_Integer>::max()))
				lua_pushinteger(L, static_cast<lua_Integer>(u));
			else
				lua_pushnumber(L, static_cast<lua_Number>(u));
			context_.submit(L);
			return true;
		}
		bool Int64(int64_t i) {
			if (sizeof(lua_Integer) >= sizeof(int64_t) || (i <= std::numeric_limits<lua_Integer>::max() && i >= std::numeric_limits<lua_Integer>::min()))
				lua_pushinteger(L, static_cast<lua_Integer>(i));
			else
				lua_pushnumber(L, static_cast<lua_Number>(i));
			context_.submit(L);
			return true;
		}
		bool Uint64(uint64_t u) {
			if (sizeof(lua_Integer) > sizeof(uint64_t) || u <= static_cast<uint64_t>(std::numeric_limits<lua_Integer>::max()))
				lua_pushinteger(L, static_cast<lua_Integer>(u));
			else
				lua_pushnumber(L, static_cast<lua_Number>(u));
			context_.submit(L);
			return true;
		}
		bool Double(double d) {
			lua_pushnumber(L, static_cast<lua_Number>(d));
			context_.submit(L);
			return true;
		}
		bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
			lua_getglobal(L, "tonumber");
			lua_pushlstring(L, str, length);
			lua_call(L, 1, 1);
			context_.submit(L);
			return true;
		}
		bool String(const char* str, rapidjson::SizeType length, bool copy) {
			lua_pushlstring(L, str, length);
			context_.submit(L);
			return true;
		}
		bool StartObject() {
			lua_createtable(L, 0, 0);							// [..., object]

			// mark as object.
			luaL_getmetatable(L, "json.object");	//[..., object, json.object]
			lua_setmetatable(L, -2);							// [..., object]

			stack_.push_back(context_);
			context_ = Ctx::Object();
			return true;
		}
		bool Key(const char* str, rapidjson::SizeType length, bool copy) const {
			lua_pushlstring(L, str, length);
			return true;
		}
		bool EndObject(rapidjson::SizeType memberCount) {
			context_ = stack_.back();
			stack_.pop_back();
			context_.submit(L);
			return true;
		}
		bool StartArray() {
			lua_createtable(L, 0, 0);

			// mark as array.
			luaL_getmetatable(L, "json.array");  //[..., array, json.array]
			lua_setmetatable(L, -2); // [..., array]

			stack_.push_back(context_);
			context_ = Ctx::Array();
			return true;
		}
		bool EndArray(rapidjson::SizeType elementCount) {
			assert(elementCount == (rapidjson::SizeType)context_.index_);
			context_ = stack_.back();
			stack_.pop_back();
			context_.submit(L);
			return true;
		}
	private:


		struct Ctx {
			Ctx() : index_(0), fn_(&topFn) {}
			Ctx(const Ctx& rhs) : index_(rhs.index_), fn_(rhs.fn_)
			{
			}
			const Ctx& operator=(const Ctx& rhs) {
				if (this != &rhs) {
					index_ = rhs.index_;
					fn_ = rhs.fn_;
				}
				return *this;
			}
			static Ctx Object() {
				return Ctx(&objectFn);
			}
			static Ctx Array()
			{
				return Ctx(&arrayFn);
			}
			void submit(lua_State* L)
			{
				fn_(L, this);
			}

			int index_;
			void(*fn_)(lua_State* L, Ctx* ctx);
		private:
			explicit Ctx(void(*f)(lua_State* L, Ctx* ctx)) : index_(0), fn_(f) {}


			static void objectFn(lua_State* L, Ctx* ctx)
			{
				lua_rawset(L, -3);
			}

			static void arrayFn(lua_State* L, Ctx* ctx)
			{
				lua_rawseti(L, -2, ++ctx->index_);
			}
			static void topFn(lua_State* L, Ctx* ctx)
			{
			}
		};

		lua_State* L;
		std::vector < Ctx > stack_;
		Ctx context_;
	};


	namespace details {
		rapidjson::Value toValue(lua_State* L, int idx, int depth, Allocator& allocator);
	}

	inline rapidjson::Value toValue(lua_State* L, int idx, Allocator& allocator) {
		return details::toValue(L, idx, 0, allocator);
	}

	inline void toDocument(lua_State* L, int idx, rapidjson::Document* doc) {
		details::toValue(L, idx, 0, doc->GetAllocator()).Swap(*doc);
	}

	inline void push(lua_State* L, const rapidjson::Value& v) {
		ToLuaHandler handler(L);
		v.Accept(handler);
	}
}

#endif // __LUA_RAPIDJSON_TOLUAHANDLER_HPP__
