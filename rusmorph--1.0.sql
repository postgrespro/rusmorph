/* contrib/rusmorph/rusmorph--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION rusmorph" to load this file. \quit

CREATE FUNCTION rusmorph_init(internal)
        RETURNS internal
        AS 'MODULE_PATHNAME'
        LANGUAGE C STRICT;

CREATE FUNCTION rusmorph_lexize(internal, internal, internal, internal)
        RETURNS internal
        AS 'MODULE_PATHNAME'
        LANGUAGE C STRICT;

CREATE TEXT SEARCH TEMPLATE rusmorph_template (
        LEXIZE = rusmorph_lexize,
	INIT   = rusmorph_init
);

CREATE TEXT SEARCH DICTIONARY rusmorph (
	TEMPLATE = rusmorph_template
);

COMMENT ON TEXT SEARCH DICTIONARY rusmorph IS 'Morphologycal analysers/taggers for Russia';
