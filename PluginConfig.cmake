#/**********************************************************\ 
# Auto-Generated Plugin Configuration file
# for IndexedDatabasePlugin
#\**********************************************************/

set(PLUGIN_NAME "IndexedDatabase")
set(PLUGIN_PREFIX "IndexedDatabase")
set(COMPANY_NAME "Brandon Haynes")

# ActiveX constants:
set(FBTYPELIB_NAME IndexedDatabasePluginLib)
set(FBTYPELIB_DESC "IndexedDatabasePlugin 1.0 Type Library")
set(IFBControl_DESC "IndexedDatabasePlugin Control Interface")
set(FBControl_DESC "IndexedDatabasePlugin Control Class")
set(IFBComJavascriptObject_DESC "IndexedDatabasePlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "IndexedDatabasePlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "IndexedDatabasePlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 178796fe-ed70-445e-bfdb-ed13752d71dc)
set(IFBControl_GUID e34b73bf-281e-4860-bde3-a68e79005108)
set(FBControl_GUID 53208bfb-a87d-4fc2-a4e0-70c69b1096b1)
set(IFBComJavascriptObject_GUID c0dcd803-e756-4a31-ba7c-5726e43d9b4f)
set(FBComJavascriptObject_GUID d799bbd0-82b2-4144-bd87-2918b488fea5)
set(IFBComEventSource_GUID 47a31460-8ba0-421c-9ef1-e88ec94411ff)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "BrandonHaynes.IndexedDatabasePlugin")
set(MOZILLA_PLUGINID "@brandonhaynes.org/IndexedDatabase")

# strings
set(FBSTRING_CompanyName "Brandon Haynes")
set(FBSTRING_FileDescription "Indexed Database - an implementation of the Indexed Database API W3C Editor's Draft")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
set(FBSTRING_LegalCopyright "Copyright 2010 Brandon Haynes")
set(FBSTRING_PluginFileName "${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "Indexed Database APIPlugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "Indexed Database APIPlugin")
set(FBSTRING_MIMEType "application/x-indexeddatabase")

add_boost_library(date_time)
add_boost_library(filesystem)
add_boost_library(system)

