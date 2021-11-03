#include <tree_sitter/parser.h>
#include <wctype.h>

// Mostly a copy paste of tree-sitter-javascript/src/scanner.c

enum TokenType {
  AUTOMATIC_SEMICOLON,
};

void *tree_sitter_kotlin_external_scanner_create() { return NULL; }
void tree_sitter_kotlin_external_scanner_destroy(void *p) {}
void tree_sitter_kotlin_external_scanner_reset(void *p) {}
unsigned tree_sitter_kotlin_external_scanner_serialize(void *p, char *buffer) { return 0; }
void tree_sitter_kotlin_external_scanner_deserialize(void *p, const char *b, unsigned n) {}

static void advance(TSLexer *lexer) { lexer->advance(lexer, false); }

static bool scan_whitespace_and_comments(TSLexer *lexer) {
  for (;;) {
    while (iswspace(lexer->lookahead)) {
      advance(lexer);
    }

    if (lexer->lookahead == '/') {
      advance(lexer);

      if (lexer->lookahead == '/') {
        advance(lexer);
        while (lexer->lookahead != 0 && lexer->lookahead != '\n') {
          advance(lexer);
        }
      } else if (lexer->lookahead == '*') {
        advance(lexer);
        while (lexer->lookahead != 0) {
          if (lexer->lookahead == '*') {
            advance(lexer);
            if (lexer->lookahead == '/') {
              advance(lexer);
              break;
            }
          } else {
            advance(lexer);
          }
        }
      } else {
        return false;
      }
    } else {
      return true;
    }
  }
}


bool tree_sitter_kotlin_external_scanner_scan(void *payload, TSLexer *lexer,
                                                  const bool *valid_symbols) {
  if (!valid_symbols[AUTOMATIC_SEMICOLON]) return false;
  lexer->result_symbol = AUTOMATIC_SEMICOLON;
  lexer->mark_end(lexer);

  for (;;) {
    //if (lexer->lookahead == 0) return true;
    //if (lexer->lookahead == '}') return true;
    //if (lexer->is_at_included_range_start(lexer)) return true;
    if (!iswspace(lexer->lookahead)) return false;
    if (lexer->lookahead == '\n') break;
    // other whitespace (not newline)
    advance(lexer);
  }
  // consume the '\n' from the break
  advance(lexer);

  if (!scan_whitespace_and_comments(lexer)) return false;

  switch (lexer->lookahead) {
    // specific to kotlin
    case '{': // ex: function body defined on next line after decl

    // from tree-sitter-javascript
    //case ',':
    case '.': // ex: method-chain calls on next line
    case ':': // ex: inheritance clause defined on next line
    //case ';':
    //case '*':
    //case '%':
    //case '>':
    //case '<':
    case '=': // ex: fun/val definition on next line (or == binop)
    //case '[':
    //case '(':
    case '?': // ex: evlis operator ?: on next line
    case '^': // ex: binary operator between 2 exprs
    case '|': // ex: binary operator between 2 exprs
    case '&': // ex: binary operator between 2 exprs
    //case '/':
      return false;
    //
    //// Insert a semicolon before `--` and `++`, but not before binary `+` or `-`.
    //case '+':
    //  advance(lexer);
    //  return lexer->lookahead == '+';
    //case '-':
    //  advance(lexer);
    //  return lexer->lookahead == '-';
    //
    //// Don't insert a semicolon before `!=`, but do insert one before a unary `!`.
    //case '!':
    //  advance(lexer);
    //  return lexer->lookahead != '=';
    //
    //// Don't insert a semicolon before `in` or `instanceof`, but do insert one
    //// before an identifier.
    //case 'i':
    //  advance(lexer);
    //
    //  if (lexer->lookahead != 'n') return true;
    //  advance(lexer);
    //
    //  if (!iswalpha(lexer->lookahead)) return false;
    //
    //  for (unsigned i = 0; i < 8; i++) {
    //    if (lexer->lookahead != "stanceof"[i]) return true;
    //    advance(lexer);
    //  }
    //
    //  if (!iswalpha(lexer->lookahead)) return false;
    //  break;
  }

  return true;
}
