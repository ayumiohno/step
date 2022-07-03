# Pythonでループ順序入れ替えによる速度差がほとんど生じない理由
* メモリアクセスではなく、構文解析が律速となるから
    * 構文解析にメモリアクセスの10~30倍の時間がかかるとしたら、メモリアクセスの連続性による差は誤差程度
    * また仮に構文解析と評価を並列/並行していれば、次の構文解析中にメモリアクセスをできるので差はほぼ生じなくなる