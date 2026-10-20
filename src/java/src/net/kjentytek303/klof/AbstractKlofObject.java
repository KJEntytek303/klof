package net.kjentytek303.klof;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Serializable;

import java.lang.Cloneable;
import java.lang.String;
import java.lang.SuppressWarnings;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.kjentytek303.klof.KlofSettings;

public class AbstractKlofObject implements Cloneable {
	public static final int NULL = 0;
	public static final int SCALAR = 1;
	public static final int STRING = 1;
	public static final int ARRAY = 2;
	public static final int STRUCT = 3;
	public static final int UNDEFINED = 4;
	public static final int INT = 0x12;
	public static final int DOUBLE = 0x1a;
	public static final int BOOLEAN = 0x0f;

	public static final int ERR_GENERAL_ERROR = 1;
	public static final int ERR_TRUNCATION = 2;
	public static final int ERR_ENFORCEMENT_MISMATCH = 3;

	//Remember, there's effectively no difference between a java Object reference, and a C void pointer
	protected Object holder = null;
	protected int type = NULL;

	//Setters
	public void setNull() {// {{{
		holder = null;
		type = NULL;
	}

	public void setScalar(String str) {
		holder = str;
		type = SCALAR;
	}

	public void setArray(List<AbstractKlofObject> arr) {
		holder = arr;
		type = ARRAY;
	}

	public void setStruct(Map<String, AbstractKlofObject> map) {
		holder = map;
		type = STRUCT;
	}

	public void setUndef() {
		holder = null;
		type = UNDEFINED;
	}

	public void setInt(int value) {
		holder = (Integer) value;
		type = INT;
	}

	public void setDouble(double value) {
		holder = (Double) value;
		type = DOUBLE;
	}

	public void setBoolean(boolean value) {
		holder = (Boolean) value;
		type = BOOLEAN;
	}// }}}

	//Unchecked abstract getter
	public int getType() { return type; }// {{{

	public Object get() { return holder; }// }}}

	@Override
	//Deep copy
	public AbstractKlofObject clone() {// {{{
		AbstractKlofObject ret = new AbstractKlofObject();

		switch (type) {
			case NULL:
			       	return ret;

			case SCALAR: 
				ret.setScalar((String)holder);
				return ret;

			case ARRAY:
				/* fuck java for making Object.clone() protected.
				if( holder instanceof Cloneable cloneable) {
					ret.setArray( (List<AbstractKlofObject>)(cloneable.clone()) );
					return ret;
				}
				*/

				ArrayList<AbstractKlofObject> new_list = new ArrayList<>();
				new_list.ensureCapacity( ((List) holder).size());

				for( AbstractKlofObject elem : (List<AbstractKlofObject>)holder ) {
					new_list.add( elem.clone());
				}

				ret.setArray(new_list);
				return ret;

			case STRUCT:
				/* fuck java for making Object.clone() protected.
				if( holder instanceof Cloneable cloneable) {
					ret.setStruct( cloneable.clone() );
					return ret;
				}
				*/

				Map<String, AbstractKlofObject> new_map = new HashMap<>();
				((Map<String, AbstractKlofObject>)holder).forEach( (key, value) -> {
					new_map.put(key, value.clone());
				});

				ret.setStruct(new_map);
				return ret;

			case UNDEFINED: 
				return null;

			case INT:
				ret.setInt( ((Integer) holder).intValue());
				return ret;

			case DOUBLE:
				ret.setDouble( ((Double) holder).doubleValue());
				return ret;

			case BOOLEAN:
				ret.setBoolean( ((Boolean) holder).booleanValue());
				return ret;
		}
		return null;
	}// }}}

//	public void writeStream( BufferedWriter ostream, /*Nullable*/ KlofSettings settings );
//	public void readStream ( BufferedReader istream, /*Nullable*/ KlofSettings settings );

//	public void readDKlofStream( BufferedReader istream, /*Nullable*/ KlofSettings settings ) {
//	}

	public int writeDKlofStream( BufferedWriter ostream ) throws IOException {
		return writeDKlofStream( ostream, null, "" );
	}
	public int writeDKlofStream( BufferedWriter ostream, /*Nullable*/ KlofSettings settings ) throws IOException {
		return writeDKlofStream( ostream, settings, "");
	}

//	@SuppressWarnings("unchecked")
	public int writeDKlofStream( BufferedWriter ostream, /*Nullable*/ KlofSettings settings, String path ) throws IOException {
		if(settings != null) {
			settings = new KlofSettings();
		}

		AtomicInteger errors;
		int errors_arr = 0;

		switch (this.type) {
			case NULL:
				if( settings.strict_types ) {
					ostream.write(path + "=null");
					ostream.newLine();
					return 0;
				} else {
					ostream.write(path + "=");
					ostream.newLine();
					return ERR_TRUNCATION;
				}

			case SCALAR:
				String str = (String) holder;
				str = str.replace("\n", "\\n");

				if( settings.strict_types ) {
					str = "\"" + str;
				} else if( shouldBackslashValueDklof(str) ) {
					str = "\\" + str;
				}
				ostream.write(path + "=" + str);
				ostream.newLine();
				return 0;
				
			case ARRAY:
				List<AbstractKlofObject> arr = (List<AbstractKlofObject>) holder;
				for( var elem : arr ) {
					errors_arr += elem.writeDKlofStream(ostream, settings, path + "[+");
				}
				return (errors_arr > 0) ? ERR_GENERAL_ERROR : 0;
				
			case STRUCT:
				errors = new AtomicInteger(0);
				final KlofSettings settings_cp = settings;
				Map<String, AbstractKlofObject> struct = (Map<String, AbstractKlofObject>) holder;

				struct.forEach( (key, elem) -> {
					try {
						int ret = elem.writeDKlofStream(ostream, settings_cp, path.equals("") ? key : "{" + key);
						errors.addAndGet(ret);
					} catch(IOException e) {
						errors.set(-1);
					}
				});
				if( errors.get() == -1 ) {
					throw new IOException();
				}

				return (errors.get() > 0) ? ERR_GENERAL_ERROR : 0;

			case UNDEFINED:
				return 0;

			case INT:
				Integer in = (Integer) holder;
				ostream.write(path + "=" + in.intValue());
				ostream.newLine();

				return settings.strict_types ? 0 : ERR_ENFORCEMENT_MISMATCH;

			case DOUBLE:
				Double db = (Double) holder;
				ostream.write(path + "=" + db.doubleValue());
				ostream.newLine();

				return settings.strict_types ? 0 : ERR_ENFORCEMENT_MISMATCH;
			case BOOLEAN:
				Boolean bool = (Boolean) holder;
				if( settings.strict_types ) {
					ostream.write(path + "=" + bool.booleanValue());
					ostream.newLine();
					return 0;
				}

				ostream.write(path + "=" + (bool.booleanValue() ? "1" : "0"));
				ostream.newLine();
				return ERR_TRUNCATION;
		}
		return ERR_GENERAL_ERROR;
	}

	/*
	@Override
	public String toString() {
		toDklofString("", new KlofSettings());
	}

	public String toDklofString(String path, KlofSettings settings) {
		if( KlofSettings != null ) {
			//print settings into the file
		}
	}

	public String toKlofString() {
		return null;
	}*/
	public static boolean shouldBackslashValueDklof(String str) {
		return str.charAt(0) == '\\' || str.charAt(0) == '[' || str.charAt(0) == '{' || str.charAt(0) == '\"';
	}

}
