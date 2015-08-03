
#if defined(__C51__)
// has no support for packing, but is packed to 1
#elif defined(__C166__)
#pragma pack()

#elif defined(__C30__)

#else

#pragma pack(pop)

#endif

#ifdef _MSC_VER_
#pragma warning(pop)		// restore warn level, see pack_push
#endif
