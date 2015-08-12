# bones


bug
1.暂不推荐在layered window中使用 richedit

	a. layered window 不能显示 richedit 光标 
       可以自绘光标但考虑到richedit的效率暂时不解决	

	b. richedit 所在窗口的颜色是半透明的情况下 如果richedit也设置了背景透明
	   部分有抗锯齿处理的字体会有周边阴影，
	   推测是 TxDraw不支持alpha 抗锯齿采样忽略了alpha导致的
       如果richedit 背景设置了不透明或者richedit所在窗口的颜色不透明则一切正常
       WIN8后可以使用TxDrawD2D, 由于需要支持WIN7 暂时不解决	   

