概要
----
MSD Radix Sort による 文字列ソートを OpenMP を用いて並列化した実装です．
const char* の配列か，std::string の配列をソートできます．

参考文献の論文に書かれている高速化手法である
Loop Fission や Super-Alphabet 等のテクニックや，
配列のコピーを減らす工夫等が導入されています．

使い方
------
sample.cc や measure.cc を見ると大体分かると思います．

コンパイル時に -fopenmp を付けないと並列化されないので注意してください．

逆に，-fopenmp を付けないで，並列化せずに使用することもできます．
そのような状況でも std::sort より有意に高速だと思います．

性能
----
measure.cc で 3 千万個のランダム文字列 (const char*) のソートの時間を測ります．
文字列の長さは 1 から 30 文字の一様分布です．

実行例::

  % g++ -O3 -fopenmp measure.cc
  % ./a.out
  ParallelStringRadixSort::Sort(0): 1.072459 sec
  ParallelStringRadixSort::Sort(1): 1.252914 sec
  ParallelStringRadixSort::Sort(2): 1.249938 sec
  std::sort(0): 24.678047 sec
  std::sort(1): 25.407672 sec
  std::sort(2): 24.269998 sec

上の実行例は Intel Core i7 CPU 960 (3.20 GHz) 上で実行しました．

余談
----
const char* 配列のソートのほうが std::string 配列のソートより数倍高速です．

文字列ソートのアルゴリズムとして Burstsort も良く知られています．
よくチューンされた変種同士でどちらが高速なのかは，
少し調べた限りでは明確には分かりませんでした．
ただ，MSD Radix Sort が苦手とする強い偏りのあるデータも Burstsort は得意のようです．

課題
----
* Algorithmic Caching を試す？

* Software Managed Buffer の導入を検討する？

* std::vector にちゃんと対応する？

  + 今でも一応 std::vector::data() を使えばソートできますが...

参考文献
--------
* MSD Radix Sort

  + Waihong Ng and Katsuhiko Kakehi. 2007. **Cache Efficient Radix Sort for String Sorting.** IEICE Trans. Fundam. Electron. Commun. Comput. Sci. E90-A, 2 (February 2007), 457-466. DOI=10.1093/ietfec/e90-a.2.457 http://dx.doi.org/10.1093/ietfec/e90-a.2.457

  + Juha Kärkkäinen and Tommi Rantala. 2008. **Engineering Radix Sort for Strings.** In Proceedings of the 15th International Symposium on String Processing and Information Retrieval (SPIRE '08), Amihood Amir, Andrew Turpin, and Alistair Moffat (Eds.). Springer-Verlag, Berlin, Heidelberg, 3-14. DOI=10.1007/978-3-540-89097-3_3 http://dx.doi.org/10.1007/978-3-540-89097-3_3

* Burstsort

  + Ranjan Sinha and Justin Zobel. 2004. **Cache-conscious sorting of large sets of strings with dynamic tries.** J. Exp. Algorithmics 9, Article 1.5 (December 2004). DOI=10.1145/1005813.1041517 http://doi.acm.org/10.1145/1005813.1041517

  + Ranjan Sinha and Anthony Wirth. 2010. **Engineering burstsort: Toward fast in-place string sorting.** J. Exp. Algorithmics 15, Article 2.5 (March 2010), 1.14 pages. DOI=10.1145/1671973.1671978 http://doi.acm.org/10.1145/1671973.1671978
