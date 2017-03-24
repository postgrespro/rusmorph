create extension rusmorph;

set client_encoding to 'utf-8';

select ts_lexize('rusmorph', 'слушать');
select ts_lexize('rusmorph', 'слушаю');
select ts_lexize('rusmorph', 'слушаюсь');
select ts_lexize('rusmorph', 'слушаясь');
select ts_lexize('rusmorph', 'слушаемся');
select ts_lexize('rusmorph', 'слушается');
select ts_lexize('rusmorph', 'слушалось');
select ts_lexize('rusmorph', 'слушались');
select ts_lexize('rusmorph', 'слушалась');
select ts_lexize('rusmorph', 'слушаешься');
select ts_lexize('rusmorph', 'слушаетесь');
select ts_lexize('rusmorph', 'слушаются');
select ts_lexize('rusmorph', 'слушался');
select ts_lexize('rusmorph', 'слушавший');

select ts_lexize('rusmorph', 'чудо');
select ts_lexize('rusmorph', 'чудес');
select ts_lexize('rusmorph', 'чудеса');
select ts_lexize('rusmorph', 'чудесам');
select ts_lexize('rusmorph', 'Чудо');

select ts_lexize('rusmorph', 'Лебедев');
select ts_lexize('rusmorph', 'лебедев');
select ts_lexize('rusmorph', 'Лебедева');
select ts_lexize('rusmorph', 'лебедева');

