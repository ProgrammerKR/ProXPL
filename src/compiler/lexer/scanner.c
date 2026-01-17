// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

void initScanner(Scanner *scanner, const char *source) {
  scanner->start = source;
  scanner->current = source;
  scanner->line = 1;
  scanner->currentColumn = 1;
  scanner->startColumn = 1;
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static bool isAtEnd(Scanner *scanner) { return *scanner->current == '\0'; }

static char advance(Scanner *scanner) {
  scanner->current++;
  scanner->currentColumn++;
  return scanner->current[-1];
}

static char peek(Scanner *scanner) { return *scanner->current; }

static char peekNext(Scanner *scanner) {
  if (isAtEnd(scanner))
    return '\0';
  return scanner->current[1];
}

static bool match(Scanner *scanner, char expected) {
  if (isAtEnd(scanner))
    return false;
  if (*scanner->current != expected)
    return false;
  scanner->current++;
  return true;
}

static void skipWhitespace(Scanner *scanner) {
  for (;;) {
    char c = peek(scanner);
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance(scanner);
      break;
    case '\n':
      scanner->line++;
      scanner->currentColumn = 1;
      advance(scanner);
      break;
    case '/':
      if (peekNext(scanner) == '/') {
        // Line comment
        while (peek(scanner) != '\n' && !isAtEnd(scanner))
          advance(scanner);
      } else if (peekNext(scanner) == '*') {
        // Block comment
        advance(scanner); // consume /
        advance(scanner); // consume *
        while (!isAtEnd(scanner)) {
          if (peek(scanner) == '\n') {
            scanner->line++;
            scanner->currentColumn = 1;
          }
          if (peek(scanner) == '*' && peekNext(scanner) == '/') {
            advance(scanner); // consume *
            advance(scanner); // consume /
            break;
          }
          advance(scanner);
        }
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

static Token makeToken(Scanner *scanner, PxTokenType type) {
  Token token;
  token.type = type;
  token.start = scanner->start;
  token.length = (int)(scanner->current - scanner->start);
  token.line = scanner->line;
  token.column = scanner->startColumn;
  return token;
}

static Token errorToken(Scanner *scanner, const char *message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner->line;
  token.column = scanner->startColumn;
  return token;
}

static Token string(Scanner *scanner) {
  while (!isAtEnd(scanner)) {
    if (peek(scanner) == '"') break;
    
    if (peek(scanner) == '\\') {
        advance(scanner); // Consume backslash
        if (isAtEnd(scanner)) break;
        // Consume escaped character immediately so it's not checked as a quote
        // Also handle newlines in escaped chars if necessary (e.g. escaped literal newline?)
        if (peek(scanner) == '\n') scanner->line++;
        advance(scanner);
        continue; 
    }

    if (peek(scanner) == '\n')
      scanner->line++;
    advance(scanner);
  }

  if (isAtEnd(scanner))
    return errorToken(scanner, "Unterminated string.");

  advance(scanner); // Closing quote
  return makeToken(scanner, TOKEN_STRING);
}

static Token number(Scanner *scanner) {
  while (isDigit(peek(scanner)))
    advance(scanner);

  // Look for fractional part
  if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
    advance(scanner); // Consume .
    while (isDigit(peek(scanner)))
      advance(scanner);
  }

  return makeToken(scanner, TOKEN_NUMBER);
}

static PxTokenType checkKeyword(Scanner *scanner, int start, int length,
                              const char *rest, PxTokenType type) {
  if (scanner->current - scanner->start == start + length &&
      memcmp(scanner->start + start, rest, length) == 0) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}

static PxTokenType identifierType(Scanner *scanner) {
  switch (scanner->start[0]) {
  case 'a':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'b':
        return checkKeyword(scanner, 2, 6, "stract", TOKEN_ABSTRACT);
      case 's':
        // "as" or "async"
        if (scanner->current - scanner->start == 2) {
          return TOKEN_AS;
        }
        return checkKeyword(scanner, 2, 3, "ync", TOKEN_ASYNC);
      case 'w':
        return checkKeyword(scanner, 2, 3, "ait", TOKEN_AWAIT);
      case 'f': // New case for 'after'
        return checkKeyword(scanner, 2, 3, "ter", TOKEN_AFTER);
      }
    }
    break;
  case 'b':
    return checkKeyword(scanner, 1, 4, "reak", TOKEN_BREAK);
  case 'c':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'a':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 's':
            return checkKeyword(scanner, 3, 1, "e", TOKEN_CASE);
          case 't':
            return checkKeyword(scanner, 3, 2, "ch", TOKEN_CATCH);
          }
        }
        break;
      case 'l':
        return checkKeyword(scanner, 2, 3, "ass", TOKEN_CLASS);
      case 'o':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 'n':
            if (scanner->current - scanner->start > 3) {
              switch (scanner->start[3]) {
              case 's':
                return checkKeyword(scanner, 4, 1, "t", TOKEN_CONST);
              case 't':
                if (scanner->current - scanner->start > 4) {
                    // conte...
                    if (scanner->current - scanner->start > 6 && scanner->start[4] == 'e' && scanner->start[5] == 'x' && scanner->start[6] == 't') {
                         return checkKeyword(scanner, 7, 0, "", TOKEN_CONTEXT);
                    }
                }
                return checkKeyword(scanner, 4, 4, "inue", TOKEN_CONTINUE);
              }
            }
            break;
          }
        }
        break;
      }
    }
    break;
  case 'd':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'e':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
            case 'c':
              return checkKeyword(scanner, 3, 2, "ay", TOKEN_DECAY); // decay
            case 'i': // distributed
              return checkKeyword(scanner, 2, 9, "stributed", TOKEN_DISTRIBUTED);
            case 'a': // dataset
              if (scanner->current - scanner->start > 6) { // dataset 7 chars
                   return checkKeyword(scanner, 2, 5, "taset", TOKEN_DATASET);
              }
              // decay check:
              return checkKeyword(scanner, 2, 3, "cay", TOKEN_DECAY); // Re-adjust precedence
              // Wait, 'd' -> 'e' -> 'c' vs 'd' -> 'a'
              // The original logic was inside 'd' -> 'e'. 
              // 'dataset' starts with 'd' -> 'a'.
              break;
            case 'f':
              if (scanner->current - scanner->start > 3) {
              switch (scanner->start[3]) {
              case 'a':
                return checkKeyword(scanner, 4, 3, "ult", TOKEN_DEFAULT);
              case 'e':
                return checkKeyword(scanner, 4, 1, "r", TOKEN_DEFER);
              }
            }
            break;
          }
        }
        break;
      case 'o':
        return checkKeyword(scanner, 2, 0, "", TOKEN_DO);
      }
    }
    break;
  case 'e':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 's':
        // es...
        return checkKeyword(scanner, 2, 6, "calate", TOKEN_ESCALATE);
      case 'l':
        return checkKeyword(scanner, 2, 2, "se", TOKEN_ELSE);
      case 'n':
        return checkKeyword(scanner, 2, 2, "um", TOKEN_ENUM);
      case 'x':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 'p':
            return checkKeyword(scanner, 3, 3, "ort", TOKEN_EXPORT);
          case 't':
            if (scanner->current - scanner->start > 3) {
                 switch(scanner->start[3]) {
                     case 'e': return checkKeyword(scanner, 4, 2, "rn", TOKEN_EXTERN);
                     case 'n': return checkKeyword(scanner, 4, 3, "nds", TOKEN_EXTENDS);
                 }
            }
            break;
          }
        }
        break;
      }
    }
    break;
  case 'f':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'a':
        return checkKeyword(scanner, 2, 3, "lse", TOKEN_FALSE);
      case 'i':
        return checkKeyword(scanner, 2, 5, "nally", TOKEN_FINALLY);
      case 'o':
        return checkKeyword(scanner, 2, 1, "r", TOKEN_FOR);
      case 'r':
        return checkKeyword(scanner, 2, 2, "om", TOKEN_FROM);
      case 'u':
        return checkKeyword(scanner, 2, 2, "nc", TOKEN_FUNC);
      }
    }
    break;
  case 'i':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'f':
        return checkKeyword(scanner, 2, 0, "", TOKEN_IF);
      case 'm':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 'l':
            return checkKeyword(scanner, 4, 7, "ements", TOKEN_IMPLEMENTS);
          }
        }
        break;
      case 'n':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 't':
            // "int" ... "inte"
            if (scanner->current - scanner->start > 3 && scanner->start[3] == 'e') {
                 if (scanner->current - scanner->start > 4) {
                     switch (scanner->start[4]) {
                         case 'r': return checkKeyword(scanner, 5, 4, "face", TOKEN_INTERFACE);
                         case 'n': return checkKeyword(scanner, 5, 1, "t", TOKEN_INTENT);
                     }
                 }
            }
            break; // Fall through
          default:
            return checkKeyword(scanner, 2, 0, "", TOKEN_IN);
          }
        }
        return TOKEN_IN;
      case 's':
        return checkKeyword(scanner, 2, 0, "", TOKEN_IS);
      }
    }
    break;
  case 'l':
    return checkKeyword(scanner, 1, 2, "et", TOKEN_LET);
  case 'm': // Added for 'match' and 'model'
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'a':
          return checkKeyword(scanner, 2, 3, "tch", TOKEN_MATCH);
      case 'o':
          return checkKeyword(scanner, 2, 3, "del", TOKEN_MODEL); // model
      case 'u':
          return checkKeyword(scanner, 2, 4, "table", TOKEN_MUTABLE);
      }
    }
    break;
  case 'n':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'a':
        return checkKeyword(scanner, 2, 4, "tive", TOKEN_NATIVE);
      case 'e':
        return checkKeyword(scanner, 2, 1, "w", TOKEN_NEW);
      case 'u':
        return checkKeyword(scanner, 2, 2, "ll", TOKEN_NULL);
      case 'o': // node
        return checkKeyword(scanner, 2, 2, "de", TOKEN_NODE);
      }
    }
    break;
  case 'p':
    if (scanner->current - scanner->start > 1) {
        switch (scanner->start[1]) {
            case 'a': return checkKeyword(scanner, 2, 5, "ckage", TOKEN_PACKAGE);
            case 'o': return checkKeyword(scanner, 2, 4, "licy", TOKEN_POLICY); // policy
            case 'u':
              if (scanner->current - scanner->start > 2) {
                  switch (scanner->start[2]) {
                      case 'b': return checkKeyword(scanner, 3, 3, "lic", TOKEN_PUBLIC);
                      case 'r': return checkKeyword(scanner, 3, 1, "e", TOKEN_PURE); // pure
                  }
              }
              break;
            case 'r':
              if (scanner->current - scanner->start > 2) { 
                  switch(scanner->start[2]) {
                      case 'e': // predict
                         return checkKeyword(scanner, 3, 4, "dict", TOKEN_PREDICT);
                      case 'i': 
                        if (scanner->current - scanner->start > 3) {
                            switch(scanner->start[3]) {
                                case 'n': return checkKeyword(scanner, 4, 1, "t", TOKEN_PRINT);
                                case 'v': return checkKeyword(scanner, 4, 3, "ate", TOKEN_PRIVATE);
                            }
                        }
                        break;
                      case 'o': return checkKeyword(scanner, 3, 6, "tected", TOKEN_PROTECTED);
                  }
              }
              break;
        }
    }
    break;
  case 'r':
    if (scanner->current - scanner->start > 1) {
        switch (scanner->start[1]) {
            case 'e':
                 // ... handled above? No conflict with return/resolver?
                 // return, resolver, recovery, resilient, restart
                 // All start with 're'
                 // resolver -> res...
                 // resilient -> res...
                 // restart -> res...
                 // return -> ret...
                 // recovery -> rec...
                 // Let's rewrite the 'r' case fully.
                 if (scanner->current - scanner->start > 2) {
                     switch (scanner->start[2]) {
                         case 'c': return checkKeyword(scanner, 3, 5, "overy", TOKEN_RECOVERY);
                         case 's':
                             if (scanner->current - scanner->start > 3) {
                                 switch (scanner->start[3]) {
                                     case 'i': return checkKeyword(scanner, 4, 5, "lient", TOKEN_RESILIENT);
                                     case 'o': return checkKeyword(scanner, 4, 4, "lver", TOKEN_RESOLVER);
                                     case 't': return checkKeyword(scanner, 4, 3, "art", TOKEN_RESTART);
                                 }
                             }
                             break;
                         case 't': return checkKeyword(scanner, 3, 3, "urn", TOKEN_RETURN);
                     }
                 }
                 break;
            case 'o':
                 return checkKeyword(scanner, 2, 6, "llback", TOKEN_ROLLBACK);
        }
    } 
    return TOKEN_IDENTIFIER;
  case 's':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'a': return checkKeyword(scanner, 2, 6, "nitize", TOKEN_SANITIZE); // sanitize
      case 't':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 'a':
            return checkKeyword(scanner, 3, 3, "tic", TOKEN_STATIC);
          case 'r':
            return checkKeyword(scanner, 3, 3, "uct", TOKEN_STRUCT);
          }
        }
        break;
      case 'u':
        return checkKeyword(scanner, 2, 3, "per", TOKEN_SUPER);
      case 'w':
        return checkKeyword(scanner, 2, 4, "itch", TOKEN_SWITCH);
      }
    }
    break;
  case 't':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'h':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 'i':
            return checkKeyword(scanner, 3, 1, "s", TOKEN_THIS);
          case 'r':
            return checkKeyword(scanner, 3, 2, "ow", TOKEN_THROW);
          }
        }
        break;
      case 'r':
        if (scanner->current - scanner->start > 2) {
          switch (scanner->start[2]) {
          case 'u':
            return checkKeyword(scanner, 3, 1, "e", TOKEN_TRUE);
          case 'a': // train
            return checkKeyword(scanner, 3, 2, "in", TOKEN_TRAIN);
          case 'y':
            return checkKeyword(scanner, 3, 0, "", TOKEN_TRY);
          }
        }
        break;
      case 'y':
        return checkKeyword(scanner, 2, 4, "peof", TOKEN_TYPEOF);
      }
    }
    break;
  case 'u':
    return checkKeyword(scanner, 1, 2, "se", TOKEN_USE);
  case 'v':
    return checkKeyword(scanner, 1, 3, "oid", TOKEN_VOID);
  case 'w':
    return checkKeyword(scanner, 1, 4, "hile", TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

static Token identifier(Scanner *scanner) {
  while (isAlpha(peek(scanner)) || isDigit(peek(scanner)))
    advance(scanner);
  return makeToken(scanner, identifierType(scanner));
}

Token scanToken(Scanner *scanner) {
  skipWhitespace(scanner);
  scanner->start = scanner->current;
  scanner->startColumn = scanner->currentColumn;

  if (isAtEnd(scanner))
    return makeToken(scanner, TOKEN_EOF);

  char c = advance(scanner);

  if (isAlpha(c))
    return identifier(scanner);
  if (isDigit(c))
    return number(scanner);

  switch (c) {
  case '(':
    return makeToken(scanner, TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(scanner, TOKEN_RIGHT_PAREN);
  case '{':
    return makeToken(scanner, TOKEN_LEFT_BRACE);
  case '}':
    return makeToken(scanner, TOKEN_RIGHT_BRACE);
  case '[':
    return makeToken(scanner, TOKEN_LEFT_BRACKET);
  case ']':
    return makeToken(scanner, TOKEN_RIGHT_BRACKET);
  case ';':
    return makeToken(scanner, TOKEN_SEMICOLON);
  case ',':
    return makeToken(scanner, TOKEN_COMMA);
  case '~':
    return makeToken(scanner, TOKEN_TILDE);
  case '?':
    if (match(scanner, '.'))
      return makeToken(scanner, TOKEN_QUESTION_DOT);
    if (match(scanner, '?'))
      return makeToken(scanner, TOKEN_QUESTION_QUESTION);
    return makeToken(scanner, TOKEN_QUESTION);
  case ':':
    return makeToken(scanner, TOKEN_COLON);
  case '.':
    return match(scanner, '.') ? makeToken(scanner, TOKEN_DOT_DOT)
                               : makeToken(scanner, TOKEN_DOT);
  case '-':
    if (match(scanner, '-'))
      return makeToken(scanner, TOKEN_MINUS_MINUS);
    if (match(scanner, '='))
      return makeToken(scanner, TOKEN_MINUS_EQUAL);
    return makeToken(scanner, TOKEN_MINUS);
  case '+':
    if (match(scanner, '+'))
      return makeToken(scanner, TOKEN_PLUS_PLUS);
    if (match(scanner, '='))
      return makeToken(scanner, TOKEN_PLUS_EQUAL);
    return makeToken(scanner, TOKEN_PLUS);
  case '/':
    return match(scanner, '=') ? makeToken(scanner, TOKEN_SLASH_EQUAL)
                               : makeToken(scanner, TOKEN_SLASH);
  case '*':
    if (match(scanner, '*')) {
      return match(scanner, '=') ? makeToken(scanner, TOKEN_STAR_STAR_EQUAL)
                                 : makeToken(scanner, TOKEN_STAR_STAR);
    }
    return match(scanner, '=') ? makeToken(scanner, TOKEN_STAR_EQUAL)
                               : makeToken(scanner, TOKEN_STAR);
  case '%':
    return match(scanner, '=') ? makeToken(scanner, TOKEN_PERCENT_EQUAL)
                               : makeToken(scanner, TOKEN_PERCENT);
  case '!':
    return match(scanner, '=') ? makeToken(scanner, TOKEN_BANG_EQUAL)
                               : makeToken(scanner, TOKEN_BANG);
  case '=':
    if (match(scanner, '='))
      return makeToken(scanner, TOKEN_EQUAL_EQUAL);
    if (match(scanner, '>'))
      return makeToken(scanner, TOKEN_ARROW);
    return makeToken(scanner, TOKEN_EQUAL);
  case '<':
    if (match(scanner, '<')) {
      return match(scanner, '=') ? makeToken(scanner, TOKEN_LESS_LESS_EQUAL)
                                 : makeToken(scanner, TOKEN_LESS_LESS);
    }
    return match(scanner, '=') ? makeToken(scanner, TOKEN_LESS_EQUAL)
                               : makeToken(scanner, TOKEN_LESS);
  case '>':
    if (match(scanner, '>')) {
      return match(scanner, '=')
                 ? makeToken(scanner, TOKEN_GREATER_GREATER_EQUAL)
                 : makeToken(scanner, TOKEN_GREATER_GREATER);
    }
    return match(scanner, '=') ? makeToken(scanner, TOKEN_GREATER_EQUAL)
                               : makeToken(scanner, TOKEN_GREATER);
  case '&':
    if (match(scanner, '&'))
      return makeToken(scanner, TOKEN_AMPERSAND_AMPERSAND);
    return match(scanner, '=') ? makeToken(scanner, TOKEN_AMPERSAND_EQUAL)
                               : makeToken(scanner, TOKEN_AMPERSAND);
  case '|':
    if (match(scanner, '|'))
      return makeToken(scanner, TOKEN_PIPE_PIPE);
    return match(scanner, '=') ? makeToken(scanner, TOKEN_PIPE_EQUAL)
                               : makeToken(scanner, TOKEN_PIPE);
  case '^':
    return match(scanner, '=') ? makeToken(scanner, TOKEN_CARET_EQUAL)
                               : makeToken(scanner, TOKEN_CARET);
  case '"':
    return string(scanner);
  case '@':
    return makeToken(scanner, TOKEN_AT);
  }

  return errorToken(scanner, "Unexpected character.");
}

