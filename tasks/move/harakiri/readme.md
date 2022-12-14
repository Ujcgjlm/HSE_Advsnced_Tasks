# Harakiri

В C++, в отличие от Rust, можно использовать объект после `std::move`. Тем не менее, так делать не стоит.

Один из интересных примеров API это вызов так называемых _callbacks_, когда вы делаете какую-то процедуру, и если всё произошло до конца, процедура вызывается в управление пользователю. Тем не менее, хочется удостовериться, что callback
может вызываться один раз, один из примеров так делать, это явно это показать:

```cpp
std::move(callback).Run()
// OR
std::move(callback)()
```

Таким синтаксисом вы показываете, что `callback` больше не используется. Если _callback_ вызывается два раза, мы хотим, чтобы
санитайзеры как минимум ругались, а как максимум компилятор не разрешал обычные вызовы `callback()`. Такая парадигма называется `OneTimeCallback`.

Как достичь правильной имплементации `OneTimeCallback`? Читайте код, тесты, вспоминайте лекцию и мем вам в подсказку.

![image.jpg](https://s.keepmeme.com/files/en_posts/20201223/delete-this-mark-zuckerberg-holding-gun-meme.jpg)
