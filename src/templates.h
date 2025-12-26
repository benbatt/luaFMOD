#define SET(type, name) \
  static int METHOD_NAME(set ## name)(lua_State *L) \
  { \
    return SET_ ## type(L, JOIN(FMOD_PREFIX, Set ## name)); \
  }

#define GET_IMPL(type, methodName, fmodName) \
  static int METHOD_NAME(methodName)(lua_State *L) \
  { \
    return GET_ ## type(L, JOIN(FMOD_PREFIX, fmodName)); \
  }

#define GET(type, name) GET_IMPL(type, get ## name, Get ## name)
#define GET_CUSTOM(type, methodName, fmodName) GET_IMPL(type, methodName, fmodName)

#define SET_FLOAT_INDEXED(name) \
  static int METHOD_NAME(set ## name)(lua_State *L) \
  { \
    return SET_float_indexed(L, JOIN(FMOD_PREFIX, Set ## name)); \
  }

#define GET_FLOAT_INDEXED(name) \
  static int METHOD_NAME(get ## name)(lua_State *L) \
  { \
    return GET_float_indexed(L, JOIN(FMOD_PREFIX, Get ## name)); \
  }

#define SET_CONSTANT(type, name) \
  static int METHOD_NAME(set ## name)(lua_State *L) \
  { \
    return SET_constant(L, #type, JOIN(FMOD_PREFIX, Set ## name)); \
  }

#define GET_CONSTANT(type, name) \
  static int METHOD_NAME(get ## name)(lua_State *L) \
  { \
      return GET_constant(L, #type, JOIN(FMOD_PREFIX, Get ## name)); \
  }

#define GET_HANDLE_INDEXED(type, name) \
  static int METHOD_NAME(get ## name)(lua_State *L) \
  { \
    return GET_ ## type ## _indexed(L, JOIN(FMOD_PREFIX, Get ## name)); \
  }

#define PUSH_float(index) lua_pushnumber(L, value ## index);
#define CHECK_float(index) (float)luaL_checknumber(L, index)

#define PUSH_int(index) lua_pushinteger(L, value ## index);
#define CHECK_int(index) luaL_checkinteger(L, index)

#define PUSH_unsigned(index) lua_pushinteger(L, value ## index);
#define CHECK_unsigned(index) luaL_checkinteger(L, index)

#define PUSH_FMOD_BOOL(index) lua_pushboolean(L, value ## index);
#define CHECK_FMOD_BOOL(index) lua_toboolean(L, index)

#define PUSH_FMOD_VECTOR(index) PUSH_STRUCT(L, FMOD_VECTOR, value ## index);
#define CHECK_FMOD_VECTOR(index) CHECK_STRUCT(L, index, FMOD_VECTOR)

#define EXPAND(...) __VA_ARGS__

#define VA_COUNT(...) DISPATCH_VARIADIC(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, __VA_ARGS__)

#define DISPATCH_VARIADIC(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) \
  DISPATCH_VARIADIC_IMPL(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1)
#define DISPATCH_VARIADIC_IMPL(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, result, ...) result

#define INCREMENT(x) INCREMENT_ ## x

#define INCREMENT_0 1
#define INCREMENT_1 2
#define INCREMENT_2 3
#define INCREMENT_3 4
#define INCREMENT_4 5
#define INCREMENT_5 6
#define INCREMENT_6 7
#define INCREMENT_7 8
#define INCREMENT_8 9
#define INCREMENT_9 10

#define PROVIDE_PAYLOAD(payload, ...) EXPAND(JOIN(PROVIDE_PAYLOAD_, VA_COUNT(__VA_ARGS__))(payload, __VA_ARGS__))

// There will always be a comma after the last macro, so just reproduce whatever follows it
#define PROVIDE_PAYLOAD_1(payload, x) x
#define PROVIDE_PAYLOAD_2(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_1(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_3(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_2(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_4(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_3(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_5(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_4(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_6(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_5(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_7(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_6(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_8(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_7(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_9(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_8(payload, __VA_ARGS__))
#define PROVIDE_PAYLOAD_10(payload, x, ...) EXPAND(x payload PROVIDE_PAYLOAD_9(payload, __VA_ARGS__))

#define INDEX INDEX_IMPL,
#define INDEX_IMPL(index, current) index

#define CURRENT CURRENT_IMPL,
#define CURRENT_IMPL(index, current) current

#define BRACKET_LEFT BRACKET_LEFT_IMPL,
#define BRACKET_LEFT_IMPL(...) (

#define BRACKET_RIGHT BRACKET_RIGHT_IMPL,
#define BRACKET_RIGHT_IMPL(...) )

#define COMMA COMMA_IMPL,
#define COMMA_IMPL(...) ,

#define BRACKETS(...) BRACKET_LEFT_IMPL, __VA_ARGS__ BRACKET_RIGHT_IMPL,
#define PREFIX_INDEX(prefix) JOIN BRACKETS(prefix COMMA INDEX)
#define PREFIX_CURRENT(prefix) JOIN BRACKETS(prefix COMMA CURRENT)

#define PASTE_FIRST_TWO(...) PASTE_FIRST_TWO_IMPL(__VA_ARGS__)
#define PASTE_FIRST_TWO_IMPL(a, b, ...) a ## b __VA_ARGS__

#define REMOVE_BRACKETS(x) PASTE_FIRST_TWO(EAT_, REMOVE_BRACKETS_IMPL x)
#define REMOVE_BRACKETS_IMPL(...) REMOVE_BRACKETS_IMPL, __VA_ARGS__
#define EAT_REMOVE_BRACKETS_IMPL

#define FIRST(x, ...) x
#define SECOND(x, y, ...) y

#define APPEND(x, y) (REMOVE_BRACKETS(x), y)
#define REMOVE_FIRST(x, ...) __VA_ARGS__

#define FOR_EACH(index, sequence, ...) EXPAND(JOIN(FOR_EACH_, VA_COUNT sequence)(index, sequence, __VA_ARGS__))

#define FOR_EACH_1(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__))
#define FOR_EACH_2(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_1(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_3(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_2(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_4(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_3(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_5(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_4(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_6(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_5(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_7(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_6(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_8(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_7(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_9(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_8(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))
#define FOR_EACH_10(index, sequence, ...) EXPAND(PROVIDE_PAYLOAD((index, FIRST sequence), __VA_ARGS__) \
    FOR_EACH_9(INCREMENT(index), (REMOVE_FIRST sequence), __VA_ARGS__))

#define READ(index, info) READ_IMPL(APPEND(info, BASIC), index)
#define READ_IMPL(info, index) JOIN(READ_, SECOND info)(FIRST info, index)

#define READ_BASIC(type, index) type JOIN(value, index) = JOIN(CHECK_, type)(index);
#define READ_HANDLE(type, index) type *JOIN(value, index) = CHECK_HANDLE(L, index, type);
#define READ_STRUCT(type, index) type *JOIN(value, index) = CHECK_STRUCT(L, index, type);

#define DECLARE(index, info) DECLARE_IMPL(APPEND(info, BASIC), index)
#define DECLARE_IMPL(info, index) JOIN(DECLARE_, SECOND info)(FIRST info, index)

#define DECLARE_BASIC(type, index) type JOIN(value, index);
#define DECLARE_HANDLE(type, index) type *JOIN(value, index);
#define DECLARE_STRUCT(type, index) type JOIN(value, index);

#define RETURN(index, info) RETURN_IMPL(APPEND(info, BASIC), index)
#define RETURN_IMPL(info, index) JOIN(RETURN_, SECOND info)(FIRST info, index)

#define RETURN_BASIC(type, index) JOIN(PUSH_, type)(index)
#define RETURN_HANDLE(type, index) PUSH_HANDLE(L, type, JOIN(value, index));
#define RETURN_STRUCT(type, index) PUSH_STRUCT(L, type, JOIN(value, index));

#define GET_MULTI(name, ...) \
  static int METHOD_NAME(get ## name)(lua_State *L) \
  { \
    GET_SELF; \
    FOR_EACH(1, (__VA_ARGS__), DECLARE, ) \
    RETURN_IF_ERROR(JOIN(FMOD_PREFIX, Get ## name)(self FOR_EACH(1, (__VA_ARGS__), COMMA &PREFIX_INDEX(value)))); \
    FOR_EACH(1, (__VA_ARGS__), RETURN, ) \
    return VA_COUNT(__VA_ARGS__); \
  }

#define SET_MULTI(name, ...) \
  static int METHOD_NAME(set ## name)(lua_State *L) \
  { \
    GET_SELF; \
    FOR_EACH(2, (__VA_ARGS__), READ, ) \
    RETURN_STATUS(JOIN(FMOD_PREFIX, Set ## name)(self FOR_EACH(2, (__VA_ARGS__), COMMA PREFIX_INDEX(value)))); \
  }

#define GET_HANDLE_INDEXED_MULTI(name, ...) \
  static int METHOD_NAME(get ## name)(lua_State *L) \
  { \
    GET_SELF; \
    int index = luaL_checkinteger(L, 2); \
    FOR_EACH(1, (__VA_ARGS__), CURRENT *PREFIX_INDEX(value);) \
    RETURN_IF_ERROR(JOIN(FMOD_PREFIX, Get ## name)(self, \
        index FOR_EACH(1, (__VA_ARGS__), COMMA &PREFIX_INDEX(value)))); \
    FOR_EACH(1, (__VA_ARGS__), PUSH_HANDLE BRACKETS(L COMMA CURRENT COMMA PREFIX_INDEX(value));) \
    return VA_COUNT(__VA_ARGS__); \
  }

#define PROPERTY(type, name) \
  SET(type, name) \
  GET(type, name)

#define PROPERTY_MULTI(name, ...) \
  SET_MULTI(name, __VA_ARGS__) \
  GET_MULTI(name, __VA_ARGS__)

#define PROPERTY_FLOAT_INDEXED(name) \
  SET_FLOAT_INDEXED(name) \
  GET_FLOAT_INDEXED(name)

#define PROPERTY_CONSTANT(type, name) \
  SET_CONSTANT(type, name) \
  GET_CONSTANT(type, name)

#define HANDLE_LIST(type, name) \
  GET(int, Num ## name ## s) \
  GET_HANDLE_INDEXED(type, name)

static int GET_float(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, float *))
{
  GET_SELF;

  float value = 0;
  RETURN_IF_ERROR(getter(self, &value));

  lua_pushnumber(L, value);

  return 1;
}

static int SET_float(lua_State *L, FMOD_RESULT F_API (*setter)(SELF_TYPE *, float))
{
  GET_SELF;

  float value = (float)luaL_checknumber(L, 2);

  RETURN_STATUS(setter(self, value));
}

static int GET_float_indexed(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, int, float *))
{
  GET_SELF;

  int index = luaL_checkinteger(L, 2);

  float value = 0;
  RETURN_IF_ERROR(getter(self, index, &value));

  lua_pushnumber(L, value);

  return 1;
}

static int SET_float_indexed(lua_State *L, FMOD_RESULT F_API (*setter)(SELF_TYPE *, int, float))
{
  GET_SELF;

  int index = luaL_checkinteger(L, 2);
  float value = (float)luaL_checknumber(L, 3);

  RETURN_STATUS(setter(self, index, value));
}

static int SET_int(lua_State *L, FMOD_RESULT F_API (*setter)(SELF_TYPE *, int))
{
  GET_SELF;

  int value = luaL_checkinteger(L, 2);

  RETURN_STATUS(setter(self, value));
}

static int GET_int(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, int *))
{
  GET_SELF;

  int value = 0;
  RETURN_IF_ERROR(getter(self, &value));

  lua_pushinteger(L, value);

  return 1;
}

static int GET_FMOD_BOOL(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, FMOD_BOOL *))
{
  GET_SELF;

  FMOD_BOOL value = 0;
  RETURN_IF_ERROR(getter(self, &value));

  lua_pushboolean(L, value);

  return 1;
}

static int SET_FMOD_BOOL(lua_State *L, FMOD_RESULT F_API (*setter)(SELF_TYPE *, FMOD_BOOL))
{
  GET_SELF;

  int value = lua_toboolean(L, 2);

  RETURN_STATUS(setter(self, value));
}

static int GET_FMOD_VECTOR(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, FMOD_VECTOR *))
{
  GET_SELF;

  FMOD_VECTOR value;
  RETURN_IF_ERROR(getter(self, &value));

  PUSH_STRUCT(L, FMOD_VECTOR, value);

  return 1;
}

static int SET_FMOD_VECTOR(lua_State *L, FMOD_RESULT F_API (*setter)(SELF_TYPE *, FMOD_VECTOR *))
{
  GET_SELF;

  FMOD_VECTOR *value = CHECK_STRUCT(L, 2, FMOD_VECTOR);

  RETURN_STATUS(setter(self, value));
}

static int GET_constant(lua_State *L, const char *type, FMOD_RESULT F_API (*getter)(SELF_TYPE *, unsigned int *))
{
  GET_SELF;

  unsigned int value;

  RETURN_IF_ERROR(getter(self, &value));

  CONSTANT_new(L, type, value);

  return 1;
}

static int SET_constant(lua_State *L, const char *type, FMOD_RESULT F_API (*setter)(SELF_TYPE *, unsigned int))
{
  GET_SELF;

  unsigned int value = *(unsigned int*)luaL_checkudata(L, 2, type);

  RETURN_STATUS(setter(self, value));
}

static int GET_FMOD_CHANNELGROUP(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, FMOD_CHANNELGROUP **))
{
  GET_SELF;

  FMOD_CHANNELGROUP *handle = NULL;

  RETURN_IF_ERROR(getter(self, &handle));

  PUSH_HANDLE(L, FMOD_CHANNELGROUP, handle);

  return 1;
}

static int GET_FMOD_SYSTEM(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, FMOD_SYSTEM **))
{
  GET_SELF;

  FMOD_SYSTEM *handle = NULL;

  RETURN_IF_ERROR(getter(self, &handle));

  PUSH_HANDLE(L, FMOD_SYSTEM, handle);

  return 1;
}

static int GET_FMOD_DSP_indexed(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, int, FMOD_DSP **))
{
  GET_SELF;

  int index = luaL_checkinteger(L, 2);

  FMOD_DSP *handle = NULL;
  RETURN_IF_ERROR(getter(self, index, &handle));

  PUSH_HANDLE(L, FMOD_DSP, handle);

  return 1;
}
