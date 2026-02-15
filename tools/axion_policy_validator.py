#!/usr/bin/env python3
import sys
import re

# Simplified tokenization: handles (, ), strings, and symbols/numbers
# Note: Enclosed in capturing group to support re.split usage while keeping tokens
TOKEN_PATTERN = re.compile(r'(\(|\)|"[^"]*"|[^\s\(\)]+)')
# Remove comments (supports both Lisp-style ;; and C-style //)
COMMENT_PATTERN = re.compile(r'(;;|//).*')

def tokenize(text):
    text = COMMENT_PATTERN.sub('', text)
    # Using re.split with a capturing group returns a list where tokens are at odd indices.
    # The separators (even indices) are the whitespace between tokens.
    # This is significantly faster (~20%) than iterating with finditer in a loop
    # and correctly handles empty string literals ("") and strings with spaces (" ").
    return TOKEN_PATTERN.split(text)[1::2]

def parse_sexpr(tokens):
    if not tokens:
        return None
    token = tokens.pop(0)
    if token == '(':
        L = []
        while tokens and tokens[0] != ')':
            L.append(parse_sexpr(tokens))
        if tokens:
            tokens.pop(0) # pop ')'
        return L
    elif token == ')':
        raise ValueError("Unexpected )")
    else:
        return token

def validate_clause(clause):
    if not isinstance(clause, list) or not clause:
        return "Invalid clause structure"

    key = clause[0]
    if key == 'tier':
        if len(clause) != 2 or not str(clause[1]).isdigit():
            return "tier requires an integer value"
    elif key in ('max-instructions', 'max-recursion', 'max-stack'):
        if len(clause) != 2 or not str(clause[1]).isdigit():
            return f"{key} requires an integer value"
    elif key == 'loop':
        for field in clause[1:]:
            if not isinstance(field, list) or len(field) != 2:
                return "loop fields must be in (key value) format"
    elif key == 'require-match-guard':
        for field in clause[1:]:
            if not isinstance(field, list) or len(field) != 2:
                return "require-match-guard fields must be in (key value) format"
    elif key == 'require-segment-event':
        for field in clause[1:]:
            if not isinstance(field, list) or len(field) != 2:
                return "require-segment-event fields must be in (key value) format"
    elif key == 'require-axion-event':
        if len(clause) < 2:
             return "require-axion-event requires at least one field"
    elif key == 'require-alignment':
        if len(clause) < 2:
             return "require-alignment requires at least one field"
    else:
        # Unknown clause - skip as per spec but maybe warn?
        pass
    return None

def validate_policy(policy):
    if not isinstance(policy, list) or not policy or policy[0] != 'policy':
        return "Root must be (policy ...)"

    for clause in policy[1:]:
        err = validate_clause(clause)
        if err:
            return err
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: axion_policy_validator.py <policy_file>")
        sys.exit(1)

    try:
        with open(sys.argv[1], 'r') as f:
            text = f.read()

        tokens = tokenize(text)
        expr = parse_sexpr(tokens)

        if not expr:
            print("Error: Empty policy file")
            sys.exit(1)

        err = validate_policy(expr)
        if err:
            print(f"Validation Error in {sys.argv[1]}: {err}")
            sys.exit(1)

        print(f"SUCCESS: Policy {sys.argv[1]} is valid.")
    except Exception as e:
        print(f"Error processing {sys.argv[1]}: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
