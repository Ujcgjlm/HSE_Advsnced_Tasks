# Buffered channel

В этой задаче вам предстоит реализовать одну из базовых структур языка Go --- канал (go chan).
Канал позволяет организовать передачу данных от приемника к источнику (например, от одной функции к другой), т.е. поддерживает следующие операции:
* _send(value)_ --- отправить value через канал.
* _recv()_ --- получить очередное значение из канала.
* _close()_ --- закрыть канал для передачи и приема.

Важнейшей особенностью такого канала является возможность сразу нескольких источников писать в один канал данные и сразу нескольким получателям их принимать без всякой дополнительной
синхронизации с их стороны. Это позволяет удобным образом организовывать вычислительные конвейеры --- каждая функция в конвейере принимает данные из одного канала и пишет результат их обработки
в другой канал для следующей функции. При этом каждая функция может исполняться сразу несколькими потоками, а разработчику даже не нужно думать о распараллеливании вычислений
(заниматься распределением данных между потоками, следить за синхронизацией) --- всю эту работу канал берет на себя.

В языке Go существует 2 типа каналов --- буферизированные и небуферизированные, которые достаточно сильно отличаются. В этой задаче речь идет о буферизированном канале. У такого канала определена
_емкость_, обозначим ее за $`n`$, а сам канал поддерживает очередь FIFO размера не более $`n`$ и работает следующим образом:
* _send(value)_ --- если текущее количество элементов в очереди равно $`n`$ (т.е. очередь заполнена), то поток блокируется до тех пор, пока в очереди не появится
свободное место. После этого `value` записывается в конец очереди.
* _recv()_ --- если очередь пуста, то поток блокируется до тех пор, пока в очередь не будет что-нибудь записано. После этого из головы очереди извлекается очередное значение и возвращается
как результат. Таким образом, каждое значение может быть получено только в 1 экземпляре, но получающий поток (из тех, что сделали `recv`) может быть любым.
* _close()_ --- закрывает канал. Если в очереди еще содержатся какие-то элементы, то канал продолжает заниматься их отправкой. Но прочие попытки сделать `send` после закрытия канала
приводят к ошибке. `recv` же после закрытия дочитывает оставшиеся в очереди элементы, а дальнейшие вызовы должны возвращать значение, сигнализирующее о закрытии канала.

Вы должны реализовать шаблонный класс `BufferedChannel` в файле `buffered_channel.h`. `recv`, как видно, должен возвращать `std::optional`, в котором нет значения, если канал был закрыт. `send` должен бросать исключение типа `std::runtime_error` при попытке отправить через закрытый канал.

### Ограничения
Время выполнения бенчмарка не должно превышать 8 секунд.