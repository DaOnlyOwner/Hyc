// Generated from e:\C++Projects\Hyc\meta\Hyra.g4 by ANTLR 4.7.1
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class HyraLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.7.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, T__10=11, T__11=12, T__12=13, Identifier=14, DecIntegerLiteral=15, 
		FloatLiteral=16, Whitespace=17, Newline=18, BlockComment=19, LineComment=20;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	public static final String[] ruleNames = {
		"T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
		"T__9", "T__10", "T__11", "T__12", "Identifier", "DIGIT", "DecIntegerLiteral", 
		"FloatLiteral", "Whitespace", "Newline", "BlockComment", "LineComment"
	};

	private static final String[] _LITERAL_NAMES = {
		null, "'('", "')'", "'|'", "'^'", "'&'", "'+'", "'-'", "'%'", "'*'", "'/'", 
		"'='", "';'", "'var'"
	};
	private static final String[] _SYMBOLIC_NAMES = {
		null, null, null, null, null, null, null, null, null, null, null, null, 
		null, null, "Identifier", "DecIntegerLiteral", "FloatLiteral", "Whitespace", 
		"Newline", "BlockComment", "LineComment"
	};
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public HyraLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "Hyra.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2\26\u0085\b\1\4\2"+
		"\t\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4"+
		"\13\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22"+
		"\t\22\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\3\2\3\2\3\3\3\3\3\4\3\4"+
		"\3\5\3\5\3\6\3\6\3\7\3\7\3\b\3\b\3\t\3\t\3\n\3\n\3\13\3\13\3\f\3\f\3\r"+
		"\3\r\3\16\3\16\3\16\3\16\3\17\6\17K\n\17\r\17\16\17L\3\20\6\20P\n\20\r"+
		"\20\16\20Q\3\21\6\21U\n\21\r\21\16\21V\3\22\3\22\3\22\3\22\3\23\6\23^"+
		"\n\23\r\23\16\23_\3\23\3\23\3\24\3\24\5\24f\n\24\3\24\5\24i\n\24\3\24"+
		"\3\24\3\25\3\25\3\25\3\25\7\25q\n\25\f\25\16\25t\13\25\3\25\3\25\3\25"+
		"\3\25\3\25\3\26\3\26\3\26\3\26\7\26\177\n\26\f\26\16\26\u0082\13\26\3"+
		"\26\3\26\3r\2\27\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\f\27\r\31"+
		"\16\33\17\35\20\37\2!\21#\22%\23\'\24)\25+\26\3\2\6\4\2C\\c|\3\2\62;\4"+
		"\2\13\13\"\"\4\2\f\f\17\17\2\u008b\2\3\3\2\2\2\2\5\3\2\2\2\2\7\3\2\2\2"+
		"\2\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21\3\2\2\2\2\23\3"+
		"\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2\2\33\3\2\2\2\2\35\3\2\2"+
		"\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2\2\2\'\3\2\2\2\2)\3\2\2\2\2+\3\2\2\2"+
		"\3-\3\2\2\2\5/\3\2\2\2\7\61\3\2\2\2\t\63\3\2\2\2\13\65\3\2\2\2\r\67\3"+
		"\2\2\2\179\3\2\2\2\21;\3\2\2\2\23=\3\2\2\2\25?\3\2\2\2\27A\3\2\2\2\31"+
		"C\3\2\2\2\33E\3\2\2\2\35J\3\2\2\2\37O\3\2\2\2!T\3\2\2\2#X\3\2\2\2%]\3"+
		"\2\2\2\'h\3\2\2\2)l\3\2\2\2+z\3\2\2\2-.\7*\2\2.\4\3\2\2\2/\60\7+\2\2\60"+
		"\6\3\2\2\2\61\62\7~\2\2\62\b\3\2\2\2\63\64\7`\2\2\64\n\3\2\2\2\65\66\7"+
		"(\2\2\66\f\3\2\2\2\678\7-\2\28\16\3\2\2\29:\7/\2\2:\20\3\2\2\2;<\7\'\2"+
		"\2<\22\3\2\2\2=>\7,\2\2>\24\3\2\2\2?@\7\61\2\2@\26\3\2\2\2AB\7?\2\2B\30"+
		"\3\2\2\2CD\7=\2\2D\32\3\2\2\2EF\7x\2\2FG\7c\2\2GH\7t\2\2H\34\3\2\2\2I"+
		"K\t\2\2\2JI\3\2\2\2KL\3\2\2\2LJ\3\2\2\2LM\3\2\2\2M\36\3\2\2\2NP\t\3\2"+
		"\2ON\3\2\2\2PQ\3\2\2\2QO\3\2\2\2QR\3\2\2\2R \3\2\2\2SU\5\37\20\2TS\3\2"+
		"\2\2UV\3\2\2\2VT\3\2\2\2VW\3\2\2\2W\"\3\2\2\2XY\5!\21\2YZ\7\60\2\2Z[\5"+
		"!\21\2[$\3\2\2\2\\^\t\4\2\2]\\\3\2\2\2^_\3\2\2\2_]\3\2\2\2_`\3\2\2\2`"+
		"a\3\2\2\2ab\b\23\2\2b&\3\2\2\2ce\7\17\2\2df\7\f\2\2ed\3\2\2\2ef\3\2\2"+
		"\2fi\3\2\2\2gi\7\f\2\2hc\3\2\2\2hg\3\2\2\2ij\3\2\2\2jk\b\24\2\2k(\3\2"+
		"\2\2lm\7\61\2\2mn\7,\2\2nr\3\2\2\2oq\13\2\2\2po\3\2\2\2qt\3\2\2\2rs\3"+
		"\2\2\2rp\3\2\2\2su\3\2\2\2tr\3\2\2\2uv\7,\2\2vw\7\61\2\2wx\3\2\2\2xy\b"+
		"\25\2\2y*\3\2\2\2z{\7\61\2\2{|\7\61\2\2|\u0080\3\2\2\2}\177\n\5\2\2~}"+
		"\3\2\2\2\177\u0082\3\2\2\2\u0080~\3\2\2\2\u0080\u0081\3\2\2\2\u0081\u0083"+
		"\3\2\2\2\u0082\u0080\3\2\2\2\u0083\u0084\b\26\2\2\u0084,\3\2\2\2\13\2"+
		"LQV_ehr\u0080\3\b\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}