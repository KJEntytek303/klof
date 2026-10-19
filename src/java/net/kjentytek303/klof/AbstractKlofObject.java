package net.kjentytek303.klof;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.Serializable;

import java.lang.Cloneable;
import java.lang.String;
import java.lang.SuppressWarnings;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


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


}
