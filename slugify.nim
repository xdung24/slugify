## Nim wrapper for anyascii C library
## Provides ASCII transliteration for Unicode characters
## Used for creating URL-safe slugs

import strutils, unicode

# C function binding
{.compile: "anyascii.c".}

proc anyascii_c(utf32: uint32, ascii: ptr cstring): csize_t {.importc: "anyascii", header: "anyascii.h".}

proc anyascii*(text: string): string =
  ## Convert Unicode string to ASCII using the anyascii C library
  ## 
  ## Example:
  ##   anyascii("café") # returns "cafe"
  ##   anyascii("北京") # returns "Bei Jing"
  result = ""
  
  for rune in text.runes:
    var asciiPtr: cstring
    let len = anyascii_c(uint32(rune), addr asciiPtr)
    
    if len > 0:
      for i in 0 ..< len:
        result.add(asciiPtr[i])

proc slugify*(text: string, separator: char = '-'): string =
  ## Convert text to URL-safe slug
  ## 
  ## - Converts Unicode to ASCII
  ## - Converts to lowercase
  ## - Replaces whitespace and special chars with separator
  ## - Removes consecutive separators
  ## - Trims separators from start/end
  ## 
  ## Example:
  ##   slugify("Hello World!") # returns "hello-world"
  ##   slugify("Café Münster") # returns "cafe-munster"
  ##   slugify("北京 2024") # returns "bei-jing-2024"
  
  # Convert Unicode to ASCII
  var ascii = anyascii(text).toLowerAscii()
  
  result = ""
  var lastWasSep = true  # Start as true to avoid leading separator
  
  for c in ascii:
    if c in {'a'..'z', '0'..'9'}:
      result.add(c)
      lastWasSep = false
    elif not lastWasSep and c in {' ', '-', '_', '.', ',', '!', '?', '/', '\\', ':', ';'}:
      result.add(separator)
      lastWasSep = true
  
  # Remove trailing separator
  if result.len > 0 and result[^1] == separator:
    result.setLen(result.len - 1)

when isMainModule:
  let input = readAll(stdin)
  echo slugify(input)
