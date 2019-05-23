var Ouroboros = Ouroboros || {};

/*-----------------------------------------------------------------------------------------
					    	JavaScript Inheritance
-----------------------------------------------------------------------------------------*/
/* Simple JavaScript Inheritance for ES 5.1
 * based on http://ejohn.org/blog/simple-javascript-inheritance/
 *  (inspired by base2 and Prototype)
 * MIT Licensed.
 */

// The base Class implementation (does nothing)
Ouroboros.Class = function(){};
// Create a new Class that inherits from this class
Ouroboros.Class.extend = function(props) {
	var _super = this.prototype;
    var fnTest = /xyz/.test(function(){xyz;}) ? /\b_super\b/ : /.*/;
	// Set up the prototype to inherit from the base class
	// (but without running the ctor constructor)
	var proto = Object.create(_super);

	// Copy the properties over onto the new prototype
	for (var name in props) {
		// Check if we're overwriting an existing function
		proto[name] = typeof props[name] === "function" &&
		typeof _super[name] == "function" && fnTest.test(props[name])
			? (function(name, fn){
				return function() {
					var tmp = this._super;

					// Add a new ._super() method that is the same method
					// but on the super-class
					this._super = _super[name];

					// The method only need to be bound temporarily, so we
					// remove it when we're done executing
					var ret = fn.apply(this, arguments);
					this._super = tmp;

					return ret;
				};
			})(name, props[name])
			: props[name];
	}

	// The new constructor
	var newClass = typeof proto.ctor === "function"
		? proto.hasOwnProperty("ctor")
			? proto.ctor // All construction is actually done in the ctor method
			: function SubClass(){ _super.ctor.apply(this, arguments); }
		: function EmptyClass(){};

	// Populate our constructed prototype object
	newClass.prototype = proto;

	// Enforce the constructor to be what we expect
	proto.constructor = newClass;

	// And make this class extendable
	newClass.extend = Ouroboros.Class.extend;

	return newClass;
};

/*
	如果ArrayBuffer没有transfer()的方法, 则为ArrayBuffer添加transfer()方法
	该方法回一个新的ArrayBuffer， 其内容�?�自oldBuffer的数�?�，并且根�?� newByteLength 的大�?�?�对数�?�进行截�?�
	�?�考:https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer/transfer
 */
if(!ArrayBuffer.transfer) {
    ArrayBuffer.transfer = function (source, length) {
        source = Object(source);
		var dest = new ArrayBuffer(length);
		
        if(!(source instanceof ArrayBuffer) || !(dest instanceof ArrayBuffer)) {
            throw new TypeError("ArrayBuffer.transfer, error: Source and destination must be ArrayBuffer instances");
		}
		
        if(dest.byteLength >= source.byteLength) {
			var buf = new Uint8Array(dest);
			buf.set(new Uint8Array(source), 0);
		}
		else {
			throw new RangeError("ArrayBuffer.transfer, error: destination has not enough space");
		}
		
		return dest;
    };
};

// export
window.Class = Ouroboros.Class;

/*-----------------------------------------------------------------------------------------
												global
-----------------------------------------------------------------------------------------*/
Ouroboros.PACKET_MAX_SIZE		 = 1500;
Ouroboros.PACKET_MAX_SIZE_TCP	 = 1460;
Ouroboros.PACKET_MAX_SIZE_UDP	 = 1472;

Ouroboros.MESSAGE_ID_LENGTH		 = 2;
Ouroboros.MESSAGE_LENGTH_LENGTH	 = 2;
Ouroboros.MESSAGE_LENGTH1_LENGTH  = 4;
Ouroboros.MESSAGE_MAX_SIZE		 = 65535;

Ouroboros.CLIENT_NO_FLOAT		 = 0;
Ouroboros.OURO_FLT_MAX			 = 3.402823466e+38;

/*-----------------------------------------------------------------------------------------
												number64bits
-----------------------------------------------------------------------------------------*/
Ouroboros.INT64 = function(lo, hi)
{
	this.lo = lo;
	this.hi = hi;
	
	this.sign = 1;
	
	if(hi >= 2147483648)
	{
		this.sign = -1;
		if(this.lo > 0)
		{
			this.lo = (4294967296 - this.lo) & 0xffffffff;
			this.hi = 4294967295 - this.hi;
		}
		else
		{
			this.lo = (4294967296 - this.lo) & 0xffffffff;
			this.hi = 4294967296 - this.hi;
		}
	}
	
	this.toString = function()
	{
		var result = "";
		
		if(this.sign < 0)
		{
			result += "-"
		}
		
		var low = this.lo.toString(16);
		var high = this.hi.toString(16);
		
		if(this.hi > 0)
		{
			result += high;
			for(var i = 8 - low.length; i > 0; --i)
			{
				result += "0";
			}
		}
		result += low;
		
		return result;
		
	}
}

Ouroboros.UINT64 = function(lo, hi)
{
	this.lo = lo;
	this.hi = hi;
	
	this.toString = function()
	{
		var low = this.lo.toString(16);
		var high = this.hi.toString(16);
		
		var result = "";
		if(this.hi > 0)
		{
			result += high;
			for(var i = 8 - low.length; i > 0; --i)
			{
				result += "0";
			}
		}
		result += low;
		return result;
	}
}

/*-----------------------------------------------------------------------------------------
												debug
-----------------------------------------------------------------------------------------*/
Ouroboros.INFO_MSG = function(s)
{
	console.info(s);
}

Ouroboros.DEBUG_MSG = function(s)
{
	console.debug(s);
}

Ouroboros.ERROR_MSG = function(s)
{
	console.error(s);
}

Ouroboros.WARNING_MSG = function(s)
{
	console.warn(s);
}

/*-----------------------------------------------------------------------------------------
												string
-----------------------------------------------------------------------------------------*/
Ouroboros.utf8ArrayToString = function(array)
{
    var out, i, len, c;
    var char2, char3;

    out = "";
    len = array.length;
    i = 0;

    while(i < len)
    {
        c = array[i++];

        switch(c >> 4)
        {
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            // 0xxxxxxx
            out += String.fromCharCode(c);
            break;
            case 12: case 13:
            // 110x xxxx   10xx xxxx
            char2 = array[i++];
            out += String.fromCharCode(((c & 0x1F) << 6) | (char2 & 0x3F));
            break;
            case 14:
                // 1110 xxxx  10xx xxxx  10xx xxxx
                char2 = array[i++];
                char3 = array[i++];
                out += String.fromCharCode(((c & 0x0F) << 12) |
                    ((char2 & 0x3F) << 6) |
                    ((char3 & 0x3F) << 0));
                break;
        }
    }
    
    return out;
}

Ouroboros.stringToUTF8Bytes = function(str) 
{
    var utf8 = [];
    for (var i=0; i < str.length; i++) {
        var charcode = str.charCodeAt(i);
        if (charcode < 0x80) utf8.push(charcode);
        else if (charcode < 0x800) {
            utf8.push(0xc0 | (charcode >> 6), 
                      0x80 | (charcode & 0x3f));
        }
        else if (charcode < 0xd800 || charcode >= 0xe000) {
            utf8.push(0xe0 | (charcode >> 12), 
                      0x80 | ((charcode>>6) & 0x3f), 
                      0x80 | (charcode & 0x3f));
        }
        // surrogate pair
        else {
            i++;
            // UTF-16 encodes 0x10000-0x10FFFF by
            // subtracting 0x10000 and splitting the
            // 20 bits of 0x0-0xFFFFF into two halves
            charcode = 0x10000 + (((charcode & 0x3ff)<<10)
                      | (str.charCodeAt(i) & 0x3ff))
            utf8.push(0xf0 | (charcode >>18), 
                      0x80 | ((charcode>>12) & 0x3f), 
                      0x80 | ((charcode>>6) & 0x3f), 
                      0x80 | (charcode & 0x3f));
        }
    }
    return utf8;
}

/*-----------------------------------------------------------------------------------------
												event
-----------------------------------------------------------------------------------------*/
Ouroboros.EventInfo = function(classinst, callbackfn)
{
	this.callbackfn = callbackfn;
	this.classinst = classinst;
}

Ouroboros.FiredEvent = function(evtName, evtInfo, ars)
{
	this.evtName = evtName;
	this.evtInfo = evtInfo;
	this.ars = ars;
}

Ouroboros.Event = function()
{
	this._events = {};
	this._isPause = false;
	this._firedEvents = [];
	
	this.register = function(evtName, classinst, strCallback)
	{
		var callbackfn = classinst[strCallback];
		if(callbackfn == undefined)
		{
			Ouroboros.ERROR_MSG('Ouroboros.Event::fire: not found strCallback(' + classinst  + ")!"+strCallback);  
			return;
		}

		var evtlst = this._events[evtName];
		if(evtlst == undefined)
		{
			evtlst = [];
			this._events[evtName] = evtlst;
		}
		
		var info = new Ouroboros.EventInfo(classinst, callbackfn);
		evtlst.push(info);
	}

	this.deregisterAll = function(classinst)
	{
		for(var itemkey in this._events)
		{
			this.deregister(itemkey, classinst);
		}
	}
	
	this.deregister = function(evtName, classinst)
	{
		var evtlst = this._events[evtName];

		if(evtlst == undefined)
		{
			return;
		}

		while(true)
		{
			var found = false;
			for(var i=0; i<evtlst.length; i++)
			{
				var info = evtlst[i];
				if(info.classinst == classinst)
				{
					evtlst.splice(i, 1);
					found = true;
					break;
				}
			}
			
			if(!found)
				break;
		}

		this.removeFiredEvent(evtName, classinst);
	}

	this.removeAllFiredEvent = function(classinst)
	{
		this.removeFiredEvent("", classinst);
	}

	this.removeFiredEvent = function(evtName, classinst)
	{
		var firedEvents = this._firedEvents;
		while(true)
		{
			var found = false;
			for(var i=0; i<firedEvents.length; i++)
			{
				var evt = firedEvents[i];
				if((evtName == "" || evt.evtName == evtName) && evt.evtInfo.classinst == classinst)
				{
					firedEvents.splice(i, 1);
					found = true;
					break;
				}
			}

			if(!found)
				break;
		}
	}
	
	this.fire = function()
	{
		if(arguments.length < 1)
		{
			//Ouroboros.ERROR_MSG('Ouroboros.Event::fire: not found eventName!');  
			return;
		}

		var evtName = arguments[0];
		var evtlst = this._events[evtName];
		
		if(evtlst == undefined)
		{
			return;			
		}
		
		var ars = [];
		for(var i=1; i<arguments.length; i++) 
			ars.push(arguments[i]);
		
		for(var i=0; i<evtlst.length; i++)
		{
			var info = evtlst[i];

			if(!this._isPause)
			{
				if(ars.length < 1)
				{
					info.callbackfn.apply(info.classinst);
				}
				else
				{
					info.callbackfn.apply(info.classinst, ars);
				}
			}
			else
			{
				var eobj = new Ouroboros.FiredEvent(evtName, info, ars);
				this._firedEvents.push(eobj);
			}
		}
	}

	this.pause = function() 
	{
		this._isPause = true;
	}

	this.resume = function()
	{
		this._isPause = false;

		var firedEvents = this._firedEvents;
		while(firedEvents.length > 0)
		{
			var evt = firedEvents.shift();
			var info = evt.evtInfo;
			var ars = evt.ars;

			if(ars.length < 1)
			{
				info.callbackfn.apply(info.classinst);
			}
			else
			{
				info.callbackfn.apply(info.classinst, ars);
			}
		}
	}

	this.clear = function()
	{
		this._events = {};
		this._firedEvents.splice(0, this._firedEvents.length);
	}
}

Ouroboros.Event = new Ouroboros.Event();

/*-----------------------------------------------------------------------------------------
												memorystream
-----------------------------------------------------------------------------------------*/

/*
	union PackFloatXType
	{
		float	fv;
		uint32	uv;
		int		iv;
	};	
*/
Ouroboros.PackFloatXType = function()
{
	this._unionData = new ArrayBuffer(4);
	this.fv = new Float32Array(this._unionData, 0, 1);
	this.uv = new Uint32Array(this._unionData, 0, 1);
	this.iv = new Int32Array(this._unionData, 0, 1);
};

Ouroboros._xPackData = new Ouroboros.PackFloatXType();
Ouroboros._yPackData = new Ouroboros.PackFloatXType();
Ouroboros._zPackData = new Ouroboros.PackFloatXType();

Ouroboros.MemoryStream = function(size_or_buffer)
{
	if(size_or_buffer instanceof ArrayBuffer)
	{
		this.buffer = size_or_buffer;
	}
	else
	{
		this.buffer = new ArrayBuffer(size_or_buffer);
	}

	this.rpos = 0;
	this.wpos = 0;
	
	//---------------------------------------------------------------------------------
	this.readInt8 = function()
	{
		var buf = new Int8Array(this.buffer, this.rpos, 1);
		this.rpos += 1;
		return buf[0];
	}

	this.readInt16 = function()
	{
		var v = this.readUint16();
		if(v >= 32768)
			v -= 65536;
		return v;
	}
		
	this.readInt32 = function()
	{
		var v = this.readUint32();
		if(v >= 2147483648)
			v -= 4294967296;
		return v;
	}

	this.readInt64 = function()
	{
		var lo = this.readInt32();
		var hi = this.readInt32();
		return new Ouroboros.INT64(lo, hi);
	}
	
	this.readUint8 = function()
	{
		var buf = new Uint8Array(this.buffer, this.rpos, 1);
		this.rpos += 1;
		return buf[0];
	}

	this.readUint16 = function()
	{
		var buf = new Uint8Array(this.buffer, this.rpos);
		this.rpos += 2;
		return ((buf[1] & 0xff) << 8) + (buf[0] & 0xff);
	}
		
	this.readUint32 = function()
	{
		var buf = new Uint8Array(this.buffer, this.rpos);
		this.rpos += 4;
		return (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	}

	this.readUint64 = function()
	{
		var lo = this.readUint32();
		var hi = this.readUint32();
		return new Ouroboros.UINT64(lo, hi);
	}
	
	this.readFloat = function()
	{
		try
		{
			var buf = new Float32Array(this.buffer, this.rpos, 1);
		}
		catch(e)
		{
			var buf = new Float32Array(this.buffer.slice(this.rpos, this.rpos + 4));
		}
		
		this.rpos += 4;
		return buf[0];
	}

	this.readDouble = function()
	{
		try
		{
			var buf = new Float64Array(this.buffer, this.rpos, 1);
		}
		catch(e)
		{
			var buf = new Float64Array(this.buffer.slice(this.rpos, this.rpos + 8), 0, 1);
		}
		
		this.rpos += 8;
		return buf[0];
	}
	
	this.readString = function()
	{
		var buf = new Uint8Array(this.buffer, this.rpos);
		var i = 0;
		var s = "";
		
		while(true)
		{
			if(buf[i] != 0)
			{
				s += String.fromCharCode(buf[i]);
			}
			else
			{
				i++;
				break;
			}
			
			i++;
			
			if(this.rpos + i >= this.buffer.byteLength)
				throw(new Error("Ouroboros.MemoryStream::readString: rpos(" + (this.rpos + i) + ")>=" + 
					this.buffer.byteLength + " overflow!"));
		}
		
		this.rpos += i;
		return s;
	}

	this.readBlob = function()
	{
		var size = this.readUint32();
		var buf = new Uint8Array(this.buffer, this.rpos, size);
		this.rpos += size;
		return buf;
	}

	this.readStream = function()
	{
		var buf = new Uint8Array(this.buffer, this.rpos, this.buffer.byteLength - this.rpos);
		this.rpos = this.buffer.byteLength;
		return new Ouroboros.MemoryStream(buf);
	}
	
	this.readPackXZ = function()
	{
		var xPackData = Ouroboros._xPackData;
		var zPackData = Ouroboros._zPackData;
		
		xPackData.fv[0] = 0.0;
		zPackData.fv[0] = 0.0;

		xPackData.uv[0] = 0x40000000;
		zPackData.uv[0] = 0x40000000;
			
		var v1 = this.readUint8();
		var v2 = this.readUint8();
		var v3 = this.readUint8();

		var data = 0;
		data |= (v1 << 16);
		data |= (v2 << 8);
		data |= v3;

		xPackData.uv[0] |= (data & 0x7ff000) << 3;
		zPackData.uv[0] |= (data & 0x0007ff) << 15;

		xPackData.fv[0] -= 2.0;
		zPackData.fv[0] -= 2.0;
	
		xPackData.uv[0] |= (data & 0x800000) << 8;
		zPackData.uv[0] |= (data & 0x000800) << 20;
			
		var data = new Array(2);
		data[0] = xPackData.fv[0];
		data[1] = zPackData.fv[0];
		return data;
	}

	this.readPackY = function()
	{
		var v = this.readUint16();
		
		var yPackData = Ouroboros._yPackData;
		yPackData.uv[0] = 0x40000000;
		yPackData.uv[0] |= (v & 0x7fff) << 12;
		yPackData.fv[0] -= 2.0;
		yPackData.uv[0] |= (v & 0x8000) << 16;
		return yPackData.fv[0];
	}
	
	//---------------------------------------------------------------------------------
	this.writeInt8 = function(v)
	{
		var buf = new Int8Array(this.buffer, this.wpos, 1);
		buf[0] = v;
		this.wpos += 1;
	}

	this.writeInt16 = function(v)
	{	
		this.writeInt8(v & 0xff);
		this.writeInt8(v >> 8 & 0xff);
	}
		
	this.writeInt32 = function(v)
	{
		for(var i=0; i<4; i++)
			this.writeInt8(v >> i * 8 & 0xff);
	}

	this.writeInt64 = function(v)
	{
		this.writeInt32(v.lo);
		this.writeInt32(v.hi);
	}
	
	this.writeUint8 = function(v)
	{
		var buf = new Uint8Array(this.buffer, this.wpos, 1);
		buf[0] = v;
		this.wpos += 1;
	}

	this.writeUint16 = function(v)
	{
		this.writeUint8(v & 0xff);
		this.writeUint8(v >> 8 & 0xff);
	}
		
	this.writeUint32 = function(v)
	{
		for(var i=0; i<4; i++)
			this.writeUint8(v >> i * 8 & 0xff);
	}

	this.writeUint64 = function(v)
	{
		this.writeUint32(v.lo);
		this.writeUint32(v.hi);
	}
	
	this.writeFloat = function(v)
	{
		try
		{
			var buf = new Float32Array(this.buffer, this.wpos, 1);
			buf[0] = v;
		}
		catch(e)
		{
			var buf = new Float32Array(1);
			buf[0] = v;
			var buf1 = new Uint8Array(this.buffer);
			var buf2 = new Uint8Array(buf.buffer);
			buf1.set(buf2, this.wpos);
		}
		
		this.wpos += 4;
	}

	this.writeDouble = function(v)
	{
		try
		{
			var buf = new Float64Array(this.buffer, this.wpos, 1);
			buf[0] = v;
		}
		catch(e)
		{
			var buf = new Float64Array(1);
			buf[0] = v;
			var buf1 = new Uint8Array(this.buffer);
			var buf2 = new Uint8Array(buf.buffer);
			buf1.set(buf2, this.wpos);
		}

		this.wpos += 8;
	}

	this.writeBlob = function(v)
	{
		var size = v.length;
		if(size + 4> this.space())
		{
			Ouroboros.ERROR_MSG("memorystream::writeBlob: no free!");
			return;
		}
		
		this.writeUint32(size);
		var buf = new Uint8Array(this.buffer, this.wpos, size);
		
		if(typeof(v) == "string")
		{
			for(var i=0; i<size; i++)
			{
				buf[i] = v.charCodeAt(i);
			}
		}
		else
		{
			for(var i=0; i<size; i++)
			{
				buf[i] = v[i];
			}
		}
		
		this.wpos += size;
	}
	
	this.writeString = function(v)
	{
		if(v.length > this.space())
		{
			Ouroboros.ERROR_MSG("memorystream::writeString: no free!");
			return;
		}
		
		var buf = new Uint8Array(this.buffer, this.wpos);
		var i = 0;
		for(var idx=0; idx<v.length; idx++)
		{
			buf[i++] = v.charCodeAt(idx);
		}
		
		buf[i++] = 0;
		this.wpos += i;
	}

	this.append = function(stream, offset, size)
	{
		if(!(stream instanceof Ouroboros.MemoryStream)) 
		{
			Ouroboros.ERROR_MSG("MemoryStream::append(): stream must be MemoryStream instances");
			return;
		}

		if(size > this.space())
		{
			this.buffer = ArrayBuffer.transfer(this.buffer, this.buffer.byteLength + size * 2);
		}

		var buf = new Uint8Array(this.buffer, this.wpos, size);
		buf.set(new Uint8Array(stream.buffer, offset, size), 0);
		this.wpos += size;
	}

	//---------------------------------------------------------------------------------
	this.readSkip = function(v)
	{
		this.rpos += v;
	}
	
	//---------------------------------------------------------------------------------
	this.space = function()
	{
		return this.buffer.byteLength - this.wpos;
	}

	//---------------------------------------------------------------------------------
	this.length = function()
	{
		return this.wpos - this.rpos;
	}

	//---------------------------------------------------------------------------------
	this.readEOF = function()
	{
		return this.buffer.byteLength - this.rpos <= 0;
	}

	//---------------------------------------------------------------------------------
	this.done = function()
	{
		this.rpos = this.wpos;
	}
	
	//---------------------------------------------------------------------------------
	this.getbuffer = function(v)
	{
		return this.buffer.slice(this.rpos, this.wpos);
	}

	//---------------------------------------------------------------------------------
	this.setbuffer = function(buffer)
	{
		this.clear();
		this.buffer = buffer;
	}

	//---------------------------------------------------------------------------------
	this.size = function()
	{
		return this.buffer.byteLength;
	}

	//---------------------------------------------------------------------------------
	this.clear = function()
	{
		this.rpos = 0;
		this.wpos = 0;

		if(this.buffer.byteLength > Ouroboros.PACKET_MAX_SIZE)
			this.buffer = new ArrayBuffer(Ouroboros.PACKET_MAX_SIZE);
	}

	this.reclaimObject = function()
	{
		this.clear();

		if(Ouroboros.MemoryStream._objects != undefined)
			Ouroboros.MemoryStream._objects.push(this);
	}
}

Ouroboros.MemoryStream.createObject = function()
{
	if(Ouroboros.MemoryStream._objects == undefined)
		Ouroboros.MemoryStream._objects = [];
  
	return Ouroboros.MemoryStream._objects.length > 0 ? Ouroboros.MemoryStream._objects.pop() : new Ouroboros.MemoryStream(Ouroboros.PACKET_MAX_SIZE_TCP);
}



/*-----------------------------------------------------------------------------------------
												bundle
-----------------------------------------------------------------------------------------*/
Ouroboros.Bundle = function()
{
	this.memorystreams = new Array();
	this.stream = Ouroboros.MemoryStream.createObject();
	
	this.numMessage = 0;
	this.messageLengthBuffer = null;
	this.messageLength = 0;
	this.msgtype = null;
	
	//---------------------------------------------------------------------------------
	this.newMessage = function(msgtype)
	{
		this.fini(false);
		
		this.msgtype = msgtype;
		this.numMessage += 1;
		
		if(this.msgtype.length == -1)
		{
			this.messageLengthBuffer = new Uint8Array(this.stream.buffer, this.stream.wpos + Ouroboros.MESSAGE_ID_LENGTH, 2);
		}
		
		this.writeUint16(msgtype.id);
		
		if(this.messageLengthBuffer)
		{
			this.writeUint16(0);
			this.messageLengthBuffer[0] = 0;
			this.messageLengthBuffer[1] = 0;
			this.messageLength = 0;
		}
	}

	//---------------------------------------------------------------------------------
	this.writeMsgLength = function(v)
	{
		if(this.messageLengthBuffer)
		{
			this.messageLengthBuffer[0] = v & 0xff;
			this.messageLengthBuffer[1] = v >> 8 & 0xff;
		}
	}
	
	//---------------------------------------------------------------------------------
	this.fini = function(issend)
	{
		if(this.numMessage > 0)
		{
			this.writeMsgLength(this.messageLength);
			if(this.stream)
				this.memorystreams.push(this.stream);

			this.stream = Ouroboros.MemoryStream.createObject();
		}
		
		if(issend)
		{
			this.messageLengthBuffer = null;
			this.numMessage = 0;
			this.msgtype = null;
		}
	}
	
	//---------------------------------------------------------------------------------
	this.send = function(network)
	{
		this.fini(true);
		
		for(var i=0; i<this.memorystreams.length; i++)
		{
			var tmpStream = this.memorystreams[i];
			network.send(tmpStream.getbuffer());
		}

		this.reclaimObject();
	}
	
	//---------------------------------------------------------------------------------
	this.checkStream = function(v)
	{
		if(v > this.stream.space())
		{
			this.memorystreams.push(this.stream);
			this.stream = Ouroboros.MemoryStream.createObject();
		}

		this.messageLength += v;
	}
	
	//---------------------------------------------------------------------------------
	this.writeInt8 = function(v)
	{
		this.checkStream(1);
		this.stream.writeInt8(v);
	}

	this.writeInt16 = function(v)
	{
		this.checkStream(2);
		this.stream.writeInt16(v);
	}
		
	this.writeInt32 = function(v)
	{
		this.checkStream(4);
		this.stream.writeInt32(v);
	}

	this.writeInt64 = function(v)
	{
		this.checkStream(8);
		this.stream.writeInt64(v);
	}
	
	this.writeUint8 = function(v)
	{
		this.checkStream(1);
		this.stream.writeUint8(v);
	}

	this.writeUint16 = function(v)
	{
		this.checkStream(2);
		this.stream.writeUint16(v);
	}
		
	this.writeUint32 = function(v)
	{
		this.checkStream(4);
		this.stream.writeUint32(v);
	}

	this.writeUint64 = function(v)
	{
		this.checkStream(8);
		this.stream.writeUint64(v);
	}
	
	this.writeFloat = function(v)
	{
		this.checkStream(4);
		this.stream.writeFloat(v);
	}

	this.writeDouble = function(v)
	{
		this.checkStream(8);
		this.stream.writeDouble(v);
	}
	
	this.writeString = function(v)
	{
		this.checkStream(v.length + 1);
		this.stream.writeString(v);
	}
	
	this.writeBlob = function(v)
	{
		this.checkStream(v.length + 4);
		this.stream.writeBlob(v);
	}

	this.clear = function()
	{
		for(var i=0; i<this.memorystreams.length; i++)
		{
			if(this.stream != this.memorystreams[i])
				this.memorystreams[i].reclaimObject();
		}

		if(this.stream)
			this.stream.clear();
		else
			this.stream = Ouroboros.MemoryStream.createObject();

		this.memorystreams = new Array();
		this.numMessage = 0;
		this.messageLengthBuffer = null;
		this.messageLength = 0;
		this.msgtype = null;
	}

	this.reclaimObject = function()
	{
		this.clear();

		if(Ouroboros.Bundle._objects != undefined)
			Ouroboros.Bundle._objects.push(this);
	}
}

Ouroboros.Bundle.createObject = function()
{
	if(Ouroboros.Bundle._objects == undefined)
		Ouroboros.Bundle._objects = [];

	return Ouroboros.Bundle._objects.length > 0 ? Ouroboros.Bundle._objects.pop() : new Ouroboros.Bundle();
}

/*-----------------------------------------------------------------------------------------
												messages
-----------------------------------------------------------------------------------------*/
Ouroboros.reader = new Ouroboros.MemoryStream(0);
Ouroboros.datatype2id = {};

Ouroboros.mappingDataType = function(writer, argType)
{
	Ouroboros.datatype2id = {};
	Ouroboros.datatype2id["STRING"] = 1;
	Ouroboros.datatype2id["STD::STRING"] = 1;

	Ouroboros.datatype2id["UINT8"] = 2;
	Ouroboros.datatype2id["BOOL"] = 2;
	Ouroboros.datatype2id["DATATYPE"] = 2;
	Ouroboros.datatype2id["CHAR"] = 2;
	Ouroboros.datatype2id["DETAIL_TYPE"] = 2;
	Ouroboros.datatype2id["ENTITYCALL_CALL_TYPE"] = 2;

	Ouroboros.datatype2id["UINT16"] = 3;
	Ouroboros.datatype2id["UNSIGNED SHORT"] = 3;
	Ouroboros.datatype2id["SERVER_ERROR_CODE"] = 3;
	Ouroboros.datatype2id["ENTITY_TYPE"] = 3;
	Ouroboros.datatype2id["ENTITY_PROPERTY_UID"] = 3;
	Ouroboros.datatype2id["ENTITY_METHOD_UID"] = 3;
	Ouroboros.datatype2id["ENTITY_SCRIPT_UID"] = 3;
	Ouroboros.datatype2id["DATATYPE_UID"] = 3;

	Ouroboros.datatype2id["UINT32"] = 4;
	Ouroboros.datatype2id["UINT"] = 4;
	Ouroboros.datatype2id["UNSIGNED INT"] = 4;
	Ouroboros.datatype2id["ARRAYSIZE"] = 4;
	Ouroboros.datatype2id["SPACE_ID"] = 4;
	Ouroboros.datatype2id["GAME_TIME"] = 4;
	Ouroboros.datatype2id["TIMER_ID"] = 4;

	Ouroboros.datatype2id["UINT64"] = 5;
	Ouroboros.datatype2id["DBID"] = 5;
	Ouroboros.datatype2id["COMPONENT_ID"] = 5;

	Ouroboros.datatype2id["INT8"] = 6;
	Ouroboros.datatype2id["COMPONENT_ORDER"] = 6;

	Ouroboros.datatype2id["INT16"] = 7;
	Ouroboros.datatype2id["SHORT"] = 7;

	Ouroboros.datatype2id["INT32"] = 8;
	Ouroboros.datatype2id["INT"] = 8;
	Ouroboros.datatype2id["ENTITY_ID"] = 8;
	Ouroboros.datatype2id["CALLBACK_ID"] = 8;
	Ouroboros.datatype2id["COMPONENT_TYPE"] = 8;

	Ouroboros.datatype2id["INT64"] = 9;

	Ouroboros.datatype2id["PYTHON"] = 10;
	Ouroboros.datatype2id["PY_DICT"] = 10;
	Ouroboros.datatype2id["PY_TUPLE"] = 10;
	Ouroboros.datatype2id["PY_LIST"] = 10;

	Ouroboros.datatype2id["BLOB"] = 11;

	Ouroboros.datatype2id["UNICODE"] = 12;

	Ouroboros.datatype2id["FLOAT"] = 13;

	Ouroboros.datatype2id["DOUBLE"] = 14;

	Ouroboros.datatype2id["VECTOR2"] = 15;

	Ouroboros.datatype2id["VECTOR3"] = 16;

	Ouroboros.datatype2id["VECTOR4"] = 17;

	Ouroboros.datatype2id["FIXED_DICT"] = 18;

	Ouroboros.datatype2id["ARRAY"] = 19;

	Ouroboros.datatype2id["ENTITYCALL"] = 20;
}

Ouroboros.mappingDataType();

Ouroboros.bindwriter = function(writer, argType)
{
	if(argType == Ouroboros.datatype2id["UINT8"])
	{
		return writer.writeUint8;
	}
	else if(argType == Ouroboros.datatype2id["UINT16"])
	{
		return writer.writeUint16;
	}
	else if(argType == Ouroboros.datatype2id["UINT32"])
	{
		return writer.writeUint32;
	}
	else if(argType == Ouroboros.datatype2id["UINT64"])
	{
		return writer.writeUint64;
	}
	else if(argType == Ouroboros.datatype2id["INT8"])
	{
		return writer.writeInt8;
	}
	else if(argType == Ouroboros.datatype2id["INT16"])
	{
		return writer.writeInt16;
	}
	else if(argType == Ouroboros.datatype2id["INT32"])
	{
		return writer.writeInt32;
	}
	else if(argType == Ouroboros.datatype2id["INT64"])
	{
		return writer.writeInt64;
	}
	else if(argType == Ouroboros.datatype2id["FLOAT"])
	{
		return writer.writeFloat;
	}
	else if(argType == Ouroboros.datatype2id["DOUBLE"])
	{
		return writer.writeDouble;
	}
	else if(argType == Ouroboros.datatype2id["STRING"])
	{
		return writer.writeString;
	}
	else if(argType == Ouroboros.datatype2id["FIXED_DICT"])
	{
		return writer.writeStream;
	}
	else if(argType == Ouroboros.datatype2id["ARRAY"])
	{
		return writer.writeStream;
	}
	else
	{
		return writer.writeStream;
	}
}

Ouroboros.bindReader = function(argType)
{
	if(argType == Ouroboros.datatype2id["UINT8"])
	{
		return Ouroboros.reader.readUint8;
	}
	else if(argType == Ouroboros.datatype2id["UINT16"])
	{
		return Ouroboros.reader.readUint16;
	}
	else if(argType == Ouroboros.datatype2id["UINT32"])
	{
		return Ouroboros.reader.readUint32;
	}
	else if(argType == Ouroboros.datatype2id["UINT64"])
	{
		return Ouroboros.reader.readUint64;
	}
	else if(argType == Ouroboros.datatype2id["INT8"])
	{
		return Ouroboros.reader.readInt8;
	}
	else if(argType == Ouroboros.datatype2id["INT16"])
	{
		return Ouroboros.reader.readInt16;
	}
	else if(argType == Ouroboros.datatype2id["INT32"])
	{
		return Ouroboros.reader.readInt32;
	}
	else if(argType == Ouroboros.datatype2id["INT64"])
	{
		return Ouroboros.reader.readInt64;
	}
	else if(argType == Ouroboros.datatype2id["FLOAT"])
	{
		return Ouroboros.reader.readFloat;
	}
	else if(argType == Ouroboros.datatype2id["DOUBLE"])
	{
		return Ouroboros.reader.readDouble;
	}
	else if(argType == Ouroboros.datatype2id["STRING"])
	{
		return Ouroboros.reader.readString;
	}
	else if(argType == Ouroboros.datatype2id["PYTHON"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["VECTOR2"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["VECTOR3"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["VECTOR4"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["BLOB"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["UNICODE"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["FIXED_DICT"])
	{
		return Ouroboros.reader.readStream;
	}
	else if(argType == Ouroboros.datatype2id["ARRAY"])
	{
		return Ouroboros.reader.readStream;
	}
	else
	{
		return Ouroboros.reader.readStream;
	}
}
	
Ouroboros.Message = function(id, name, length, argstype, args, handler)
{
	this.id = id;
	this.name = name;
	this.length = length;
	this.argsType = argstype;
	
	// 绑定执行
	for(var i=0; i<args.length; i++)
	{
		args[i] = Ouroboros.bindReader(args[i]);
	}
	
	this.args = args;
	this.handler = handler;
	
	this.createFromStream = function(msgstream)
	{
		if(this.args.length <= 0)
			return msgstream;
		
		var result = new Array(this.args.length);
		for(var i=0; i<this.args.length; i++)
		{
			result[i] = this.args[i].call(msgstream);
		}
		
		return result;
	}
	
	this.handleMessage = function(msgstream)
	{
		if(this.handler == null)
		{
			Ouroboros.ERROR_MSG("Ouroboros.Message::handleMessage: interface(" + this.name + "/" + this.id + ") no implement!");  
			return;
		}

		if(this.args.length <= 0)
		{
			if(this.argsType < 0)
				this.handler(msgstream);
			else
				this.handler();
		}
		else
		{
			this.handler.apply(Ouroboros.app, this.createFromStream(msgstream));
		}
	}
}

// 上行消�?�
Ouroboros.messages = {};
Ouroboros.messages["loginapp"] = {};
Ouroboros.messages["baseapp"] = {};
Ouroboros.clientmessages = {};

Ouroboros.messages["Loginapp_importClientMessages"] = new Ouroboros.Message(5, "importClientMessages", 0, 0, new Array(), null);
Ouroboros.messages["Baseapp_importClientMessages"] = new Ouroboros.Message(207, "importClientMessages", 0, 0, new Array(), null);
Ouroboros.messages["Baseapp_importClientEntityDef"] = new Ouroboros.Message(208, "importClientEntityDef", 0, 0, new Array(), null);
Ouroboros.messages["onImportClientMessages"] = new Ouroboros.Message(518, "onImportClientMessages", -1, -1, new Array(), null);

Ouroboros.bufferedCreateEntityMessages = {};

/*-----------------------------------------------------------------------------------------
												math
-----------------------------------------------------------------------------------------*/
Ouroboros.Vector2 = Ouroboros.Class.extend(
{
		ctor:function (x, y) {
			this.x = x;
			this.y = y;
			return true;
		},
	
		distance : function(pos)
		{
			var x = pos.x - this.x;
			var y = pos.y - this.y;
			return Math.sqrt(x * x + y * y);
		},

		add : function(vec3)
		{
			this.x += vec3.x;
			this.y += vec3.y;
			return this;
		},

		sub: function(vec3)
		{
			this.x -= vec3.x;
			this.y -= vec3.y;
			return this;
		},

		mul: function(num)
		{
			this.x *= num;
			this.y *= num;
			return this;
		},

		div: function(num)
		{
			this.x /= num;
			this.y /= num;
			return this;
		},

		neg: function()
		{
			this.x = -this.x;
			this.y = -this.y;
			return this;
		}
});

Ouroboros.Vector3 = Ouroboros.Class.extend(
{
    ctor:function (x, y, z) {
		this.x = x;
		this.y = y;
		this.z = z;
        return true;
    },

    distance : function(pos)
    {
    	var x = pos.x - this.x;
    	var y = pos.y - this.y;
    	var z = pos.z - this.z;
    	return Math.sqrt(x * x + y * y + z * z);
	},
	
	//�?��?加法
	add : function(vec3)
	{
		this.x += vec3.x;
		this.y += vec3.y;
		this.z += vec3.z;
		return this;
	},

	//�?��?�?法
	sub: function(vec3)
	{
		this.x -= vec3.x;
		this.y -= vec3.y;
		this.z -= vec3.z;
		return this;
	},

	//�?��?乘法
	mul: function(num)
	{
		this.x *= num;
		this.y *= num;
		this.z *= num;
		return this;
	},

	//�?��?除法
	div: function(num)
	{
		this.x /= num;
		this.y /= num;
		this.z /= num;
		return this;
	},

	// �?��?�?��??
	neg: function()
	{
		this.x = -this.x;
		this.y = -this.y;
		this.z = -this.z;
		return this;
	}
});

Ouroboros.Vector4 = Ouroboros.Class.extend(
{
	ctor:function (x, y, z, w) {
		this.x = x;
		this.y = y;
		this.z = z;
		this.w = w;
		return true;
	},

	distance : function(pos)
	{
		var x = pos.x - this.x;
		var y = pos.y - this.y;
		var z = pos.z - this.z;
		var w = pos.w - this.w;
		return Math.sqrt(x * x + y * y + z * z + w * w );
	},

	add : function(vec4)
	{
		this.x += vec4.x;
		this.y += vec4.y;
		this.z += vec4.z;
		this.w += vec4.w;
		return this;
	},

	sub: function(vec4)
	{
		this.x -= vec4.x;
		this.y -= vec4.y;
		this.z -= vec4.z;
		this.w -= vec4.w;
		return this;
	},

	mul: function(num)
	{
		this.x *= num;
		this.y *= num;
		this.z *= num;
		this.w *= num;
		return this;
	},

	div: function(num)
	{
		this.x /= num;
		this.y /= num;
		this.z /= num;
		this.w /= num;
		return this;
	},

	neg: function()
	{
		this.x = -this.x;
		this.y = -this.y;
		this.z = -this.z;
		this.w = -this.w;
		return this;
	}
});

Ouroboros.clampf = function (value, min_inclusive, max_inclusive) 
{
    if (min_inclusive > max_inclusive) {
        var temp = min_inclusive;
        min_inclusive = max_inclusive;
        max_inclusive = temp;
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
};

Ouroboros.int82angle = function(angle/*int8*/, half/*bool*/)
{
	return angle * (Math.PI / (half ? 254.0 : 128.0));
};

Ouroboros.angle2int8 = function(v/*float*/, half/*bool*/)
{
	var angle = 0;
	if(!half)
	{
		angle = Math.floor((v * 128.0) / float(Math.PI) + 0.5);
	}
	else
	{
		angle = Ouroboros.clampf(floorf( (v * 254.0) / float(Math.PI) + 0.5), -128.0, 127.0);
	}

	return angle;
};

/*-----------------------------------------------------------------------------------------
												entity
-----------------------------------------------------------------------------------------*/
Ouroboros.Entity = Ouroboros.Class.extend(
{
    ctor:function () {
		this.id = 0;
		this.className = "";
		this.position = new Ouroboros.Vector3(0.0, 0.0, 0.0);
		this.direction = new Ouroboros.Vector3(0.0, 0.0, 0.0);
		this.velocity = 0.0
			
		this.cell = null;
		this.base = null;
		
		// enterworld之�?�设置为true
		this.inWorld = false;
		
		// __init__调用之�?�设置为true
		this.inited = false;
		
		// 是�?�被控制
		this.isControlled = false;

		this.entityLastLocalPos = new Ouroboros.Vector3(0.0, 0.0, 0.0);
		this.entityLastLocalDir = new Ouroboros.Vector3(0.0, 0.0, 0.0);
		
		// 玩家是�?�在地�?�上
		this.isOnGround = false;

        return true;
    },
    
    // 与�?务端实体脚本中__init__类似, 代表�?始化实体
	__init__ : function()
	{
	},

	callPropertysSetMethods : function()
	{
		var currModule = Ouroboros.moduledefs[this.className];
		for(var name in currModule.propertys)
		{
			var propertydata = currModule.propertys[name];
			var properUtype = propertydata[0];
			var name = propertydata[2];
			var setmethod = propertydata[5];
			var flags = propertydata[6];
			var oldval = this[name];
			
			if(setmethod != null)
			{
				// base类属性或者进入世界�?�cell类属性会触�?�set_*方法
				// ED_FLAG_BASE_AND_CLIENT�?ED_FLAG_BASE
				if(flags == 0x00000020 || flags == 0x00000040)
				{
					if(this.inited && !this.inWorld)
						setmethod.call(this, oldval);
				}
				else
				{
					if(this.inWorld)
					{
						if(flags == 0x00000008 || flags == 0x00000010)
						{
							if(!this.isPlayer())
								continue;
						}
						
						setmethod.call(this, oldval);
					}
				}
			}
		};
	},

	onDestroy : function()
	{
	},

	onControlled : function(bIsControlled)
	{
	},

	isPlayer : function()
	{
		return this.id == Ouroboros.app.entity_id;
	},

	baseCall : function()
	{
		if(arguments.length < 1)
		{
			Ouroboros.ERROR_MSG('Ouroboros.Entity::baseCall: not fount interfaceName!');  
			return;
		}

		if(this.base == undefined)
		{
			Ouroboros.ERROR_MSG('Ouroboros.Entity::baseCall: base is None!');  
			return;			
		}
		
		var method = Ouroboros.moduledefs[this.className].base_methods[arguments[0]];

		if(method == undefined)
		{
			Ouroboros.ERROR_MSG("Ouroboros.Entity::baseCall: The server did not find the def_method(" + this.className + "." + arguments[0] + ")!");
			return;
		}
		
		var methodID = method[0];
		var args = method[3];
		
		if(arguments.length - 1 != args.length)
		{
			Ouroboros.ERROR_MSG("Ouroboros.Entity::baseCall: args(" + (arguments.length - 1) + "!= " + args.length + ") size is error!");  
			return;
		}
		
		this.base.newCall();
		this.base.bundle.writeUint16(methodID);
		
		try
		{
			for(var i=0; i<args.length; i++)
			{
				if(args[i].isSameType(arguments[i + 1]))
				{
					args[i].addToStream(this.base.bundle, arguments[i + 1]);
				}
				else
				{
					throw new Error("Ouroboros.Entity::baseCall: arg[" + i + "] is error!");
				}
			}
		}
		catch(e)
		{
			Ouroboros.ERROR_MSG(e.toString());
			Ouroboros.ERROR_MSG('Ouroboros.Entity::baseCall: args is error!');
			this.base.bundle = null;
			return;
		}
		
		this.base.sendCall();
	},
	
	cellCall : function()
	{
		if(arguments.length < 1)
		{
			Ouroboros.ERROR_MSG('Ouroboros.Entity::cellCall: not fount interfaceName!');  
			return;
		}
		
		if(this.cell == undefined)
		{
			Ouroboros.ERROR_MSG('Ouroboros.Entity::cellCall: cell is None!');  
			return;			
		}
		
		var method = Ouroboros.moduledefs[this.className].cell_methods[arguments[0]];
		
		if(method == undefined)
		{
			Ouroboros.ERROR_MSG("Ouroboros.Entity::cellCall: The server did not find the def_method(" + this.className + "." + arguments[0] + ")!");
			return;
		}
		
		var methodID = method[0];
		var args = method[3];
		
		if(arguments.length - 1 != args.length)
		{
			Ouroboros.ERROR_MSG("Ouroboros.Entity::cellCall: args(" + (arguments.length - 1) + "!= " + args.length + ") size is error!");  
			return;
		}
		
		this.cell.newCall();
		this.cell.bundle.writeUint16(methodID);
		
		try
		{
			for(var i=0; i<args.length; i++)
			{
				if(args[i].isSameType(arguments[i + 1]))
				{
					args[i].addToStream(this.cell.bundle, arguments[i + 1]);
				}
				else
				{
					throw new Error("Ouroboros.Entity::cellCall: arg[" + i + "] is error!");
				}
			}
		}
		catch(e)
		{
			Ouroboros.ERROR_MSG(e.toString());
			Ouroboros.ERROR_MSG('Ouroboros.Entity::cellCall: args is error!');
			this.cell.bundle = null;
			return;
		}
		
		this.cell.sendCall();
	},
	
	enterWorld : function()
	{
		Ouroboros.INFO_MSG(this.className + '::enterWorld: ' + this.id); 
		this.inWorld = true;
		this.onEnterWorld();
		
		Ouroboros.Event.fire(Ouroboros.EventTypes.onEnterWorld, this);
	},

	onEnterWorld : function()
	{
	},
		
	leaveWorld : function()
	{
		Ouroboros.INFO_MSG(this.className + '::leaveWorld: ' + this.id); 
		this.inWorld = false;
		this.onLeaveWorld();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onLeaveWorld, this);
	},

	onLeaveWorld : function()
	{
	},
		
	enterSpace : function()
	{
		Ouroboros.INFO_MSG(this.className + '::enterSpace: ' + this.id); 
		this.onEnterSpace();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onEnterSpace, this);
		
		// �?立�?�刷新表现层对象的�?置
		Ouroboros.Event.fire(Ouroboros.EventTypes.set_position, this);
		Ouroboros.Event.fire(Ouroboros.EventTypes.set_direction, this);
	},

	onEnterSpace : function()
	{
	},
		
	leaveSpace : function()
	{
		Ouroboros.INFO_MSG(this.className + '::leaveSpace: ' + this.id); 
		this.onLeaveSpace();
		Ouroboros.Event.fire("onLeaveSpace", this);
	},

	onLeaveSpace : function()
	{
	},
		
	set_position : function(old)
	{
		// Ouroboros.DEBUG_MSG(this.className + "::set_position: " + old);  
		
		if(this.isPlayer())
		{
			Ouroboros.app.entityServerPos.x = this.position.x;
			Ouroboros.app.entityServerPos.y = this.position.y;
			Ouroboros.app.entityServerPos.z = this.position.z;
		}
		
		Ouroboros.Event.fire(Ouroboros.EventTypes.set_position, this);
	},

	onUpdateVolatileData : function()
	{
	},
	
	set_direction : function(old)
	{
		// Ouroboros.DEBUG_MSG(this.className + "::set_direction: " + old);  
		Ouroboros.Event.fire(Ouroboros.EventTypes.set_direction, this);
	}
});

/*-----------------------------------------------------------------------------------------
												EntityCall
-----------------------------------------------------------------------------------------*/
Ouroboros.ENTITYCALL_TYPE_CELL = 0;
Ouroboros.ENTITYCALL_TYPE_BASE = 1;

Ouroboros.EntityCall = function()
{
	this.id = 0;
	this.className = "";
	this.type = Ouroboros.ENTITYCALL_TYPE_CELL;
	this.networkInterface = Ouroboros.app;
	
	this.bundle = null;
	
	this.isBase = function()
	{
		return this.type == Ouroboros.ENTITYCALL_TYPE_BASE;
	}

	this.isCell = function()
	{
		return this.type == Ouroboros.ENTITYCALL_TYPE_CELL;
	}
	
	this.newCall = function()
	{  
		if(this.bundle == null)
			this.bundle = Ouroboros.Bundle.createObject();
		
		if(this.type == Ouroboros.ENTITYCALL_TYPE_CELL)
			this.bundle.newMessage(Ouroboros.messages.Baseapp_onRemoteCallCellMethodFromClient);
		else
			this.bundle.newMessage(Ouroboros.messages.Entity_onRemoteMethodCall);

		this.bundle.writeInt32(this.id);
		
		return this.bundle;
	}
	
	this.sendCall = function(bundle)
	{
		if(bundle == undefined)
			bundle = this.bundle;
		
		bundle.send(this.networkInterface);
		
		if(this.bundle == bundle)
			this.bundle = null;
	}
}

/*-----------------------------------------------------------------------------------------
												entitydef
-----------------------------------------------------------------------------------------*/
Ouroboros.moduledefs = {};
Ouroboros.datatypes = {};

Ouroboros.DATATYPE_UINT8 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readUint8.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeUint8(v);
	}

	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		if(typeof(v) != "number")
		{
			return false;
		}
		
		if(v < 0 || v > 0xff)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_UINT16 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readUint16.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeUint16(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		if(typeof(v) != "number")
		{
			return false;
		}
		
		if(v < 0 || v > 0xffff)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_UINT32 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readUint32.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeUint32(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		if(typeof(v) != "number")
		{
			return false;
		}
		
		if(v < 0 || v > 0xffffffff)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_UINT64 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readUint64.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeUint64(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		return v instanceof Ouroboros.UINT64;
	}
}

Ouroboros.DATATYPE_INT8 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readInt8.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeInt8(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		if(typeof(v) != "number")
		{
			return false;
		}
		
		if(v < -0x80 || v > 0x7f)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_INT16 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readInt16.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeInt16(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		if(typeof(v) != "number")
		{
			return false;
		}
		
		if(v < -0x8000 || v > 0x7fff)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_INT32 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readInt32.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeInt32(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		if(typeof(v) != "number")
		{
			return false;
		}
		
		if(v < -0x80000000 || v > 0x7fffffff)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_INT64 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readInt64.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeInt64(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseInt(v);
	}
	
	this.isSameType = function(v)
	{
		return v instanceof Ouroboros.INT64;
	}
}

Ouroboros.DATATYPE_FLOAT = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readFloat.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeFloat(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseFloat(v);
	}
	
	this.isSameType = function(v)
	{
		return typeof(v) == "number";
	}
}

Ouroboros.DATATYPE_DOUBLE = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readDouble.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeDouble(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return parseFloat(v);
	}
	
	this.isSameType = function(v)
	{
		return typeof(v) == "number";
	}
}

Ouroboros.DATATYPE_STRING = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return Ouroboros.reader.readString.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeString(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		if(typeof(v) == "string")
			return v;
		
		return "";
	}
	
	this.isSameType = function(v)
	{
		return typeof(v) == "string";
	}
}

Ouroboros.DATATYPE_VECTOR2 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		if(Ouroboros.CLIENT_NO_FLOAT)
		{
			var x = Ouroboros.reader.readInt32.call(stream);
			var y = Ouroboros.reader.readInt32.call(stream);
			return new Ouroboros.Vector2(x, y);
		}
		else
		{
			var x = Ouroboros.reader.readFloat.call(stream);
			var y = Ouroboros.reader.readFloat.call(stream);
			return new Ouroboros.Vector2(x, y);
		}
		
		return undefined;
	}
	
	this.addToStream = function(stream, v)
	{
		if(Ouroboros.CLIENT_NO_FLOAT)
		{
			stream.writeInt32(v.x);
			stream.writeInt32(v.y);
		}
		else
		{
			stream.writeFloat(v.x);
			stream.writeFloat(v.y);			
		}
	}
	
	this.parseDefaultValStr = function(v)
	{
		return new Ouroboros.Vector2(0.0, 0.0);
	}
	
	this.isSameType = function(v)
	{
		if(! v instanceof Ouroboros.Vector2)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_VECTOR3 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		if(Ouroboros.CLIENT_NO_FLOAT)
		{
			var x = Ouroboros.reader.readInt32.call(stream);
			var y = Ouroboros.reader.readInt32.call(stream);
			var z = Ouroboros.reader.readInt32.call(stream);
			return new Ouroboros.Vector3(x, y, z);
		}
		else
		{
			var x = Ouroboros.reader.readFloat.call(stream);
			var y = Ouroboros.reader.readFloat.call(stream);
			var z = Ouroboros.reader.readFloat.call(stream);
			return new Ouroboros.Vector3(x, y, z);
		}
	
		return undefined;
	}
	
	this.addToStream = function(stream, v)
	{
		if(Ouroboros.CLIENT_NO_FLOAT)
		{
			stream.writeInt32(v.x);
			stream.writeInt32(v.y);
			stream.writeInt32(v.z);
		}
		else
		{
			stream.writeFloat(v.x);
			stream.writeFloat(v.y);
			stream.writeFloat(v.z);
		}
	}
	
	this.parseDefaultValStr = function(v)
	{
		return new Ouroboros.Vector3(0.0, 0.0, 0.0);
	}
	
	this.isSameType = function(v)
	{
		if(! v instanceof Ouroboros.Vector3)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_VECTOR4 = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		if(Ouroboros.CLIENT_NO_FLOAT)
		{
			var x = Ouroboros.reader.readInt32.call(stream);
			var y = Ouroboros.reader.readInt32.call(stream);
			var z = Ouroboros.reader.readInt32.call(stream);
			var w = Ouroboros.reader.readInt32.call(stream);
			return new Ouroboros.Vector4(x, y, z, w);
		}
		else
		{
			var x = Ouroboros.reader.readFloat.call(stream);
			var y = Ouroboros.reader.readFloat.call(stream);
			var z = Ouroboros.reader.readFloat.call(stream);
			var w = Ouroboros.reader.readFloat.call(stream);
			return new Ouroboros.Vector4(x, y, z, w);
		}
		
		return undefined;
	}
	
	this.addToStream = function(stream, v)
	{
		if(Ouroboros.CLIENT_NO_FLOAT)
		{
			stream.writeInt32(v.x);
			stream.writeInt32(v.y);
			stream.writeInt32(v.z);
			stream.writeInt32(v.w);
		}
		else
		{
			stream.writeFloat(v.x);
			stream.writeFloat(v.y);		
			stream.writeFloat(v.z);
			stream.writeFloat(v.w);		
		}
	}
	
	this.parseDefaultValStr = function(v)
	{
		return new Ouroboros.Vector4(0.0, 0.0, 0.0, 0.0);
	}
	
	this.isSameType = function(v)
	{
		if(! v instanceof Ouroboros.Vector4)
		{
			return false;
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_PYTHON = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		return stream.readBlob();
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeBlob(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return new Uint8Array();
	}
	
	this.isSameType = function(v)
	{
		return false;
	}
}

Ouroboros.DATATYPE_UNICODE = function()
{
	this.bind = function()
	{
	}

	this.createFromStream = function(stream)
	{
		return Ouroboros.utf8ArrayToString(Ouroboros.reader.readBlob.call(stream));
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeBlob(Ouroboros.stringToUTF8Bytes(v));
	}
	
	this.parseDefaultValStr = function(v)
	{
		if(typeof(v) == "string")
			return v;
		
		return "";
	}
	
	this.isSameType = function(v)
	{
		return typeof(v) == "string";
	}
}

Ouroboros.DATATYPE_ENTITYCALL = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		var cid = Ouroboros.reader.readUint64.call(stream);
		var id = Ouroboros.reader.readInt32.call(stream);
		var type = Ouroboros.reader.readUint16.call(stream);
		var utype = Ouroboros.reader.readUint16.call(stream);
	}
	
	this.addToStream = function(stream, v)
	{
		var cid = new Ouroboros.UINT64(0, 0);
		var id = 0;
		var type = 0;
		var utype = 0;

		stream.writeUint64(cid);
		stream.writeInt32(id);
		stream.writeUint16(type);
		stream.writeUint16(utype);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return new Uint8Array();
	}
	
	this.isSameType = function(v)
	{
		return false;
	}
}

Ouroboros.DATATYPE_BLOB = function()
{
	this.bind = function()
	{
	}
	
	this.createFromStream = function(stream)
	{
		var size = Ouroboros.reader.readUint32.call(stream);
		var buf = new Uint8Array(stream.buffer, stream.rpos, size);
		stream.rpos += size;
		return buf;
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeBlob(v);
	}
	
	this.parseDefaultValStr = function(v)
	{
		return new Uint8Array();
	}
	
	this.isSameType = function(v)
	{
		return true;
	}
}

Ouroboros.DATATYPE_ARRAY = function()
{
	this.type = null;
	
	this.bind = function()
	{
		if(typeof(this.type) == "number")
			this.type = Ouroboros.datatypes[this.type];
	}
	
	this.createFromStream = function(stream)
	{
		var size = stream.readUint32();
		var datas = [];
		
		while(size > 0)
		{
			size--;
			datas.push(this.type.createFromStream(stream));
		};
		
		return datas;
	}
	
	this.addToStream = function(stream, v)
	{
		stream.writeUint32(v.length);
		for(var i=0; i<v.length; i++)
		{
			this.type.addToStream(stream, v[i]);
		}
	}
	
	this.parseDefaultValStr = function(v)
	{
		return [];
	}
	
	this.isSameType = function(v)
	{
		for(var i=0; i<v.length; i++)
		{
			if(!this.type.isSameType(v[i]))
			{
				return false;
			}
		}
		
		return true;
	}
}

Ouroboros.DATATYPE_FIXED_DICT = function()
{
	this.dicttype = {};
	this.implementedBy = null;
	
	this.bind = function()
	{
		for(var itemkey in this.dicttype)
		{
			var utype = this.dicttype[itemkey];
			
			if(typeof(this.dicttype[itemkey]) == "number")
				this.dicttype[itemkey] = Ouroboros.datatypes[utype];
		}
	}
	
	this.createFromStream = function(stream)
	{
		var datas = {};
		for(var itemkey in this.dicttype)
		{
			datas[itemkey] = this.dicttype[itemkey].createFromStream(stream);
		}
		
		return datas;
	}
	
	this.addToStream = function(stream, v)
	{
		for(var itemkey in this.dicttype)
		{
			this.dicttype[itemkey].addToStream(stream, v[itemkey]);
		}
	}
	
	this.parseDefaultValStr = function(v)
	{
		return {};
	}
	
	this.isSameType = function(v)
	{
		for(var itemkey in this.dicttype)
		{
			if(!this.dicttype[itemkey].isSameType(v[itemkey]))
			{
				return false;
			}
		}
		
		return true;
	}
}

Ouroboros.datatypes["UINT8"]		= new Ouroboros.DATATYPE_UINT8();
Ouroboros.datatypes["UINT16"]	= new Ouroboros.DATATYPE_UINT16();
Ouroboros.datatypes["UINT32"]	= new Ouroboros.DATATYPE_UINT32();
Ouroboros.datatypes["UINT64"]	= new Ouroboros.DATATYPE_UINT64();

Ouroboros.datatypes["INT8"]		= new Ouroboros.DATATYPE_INT8();
Ouroboros.datatypes["INT16"]		= new Ouroboros.DATATYPE_INT16();
Ouroboros.datatypes["INT32"]		= new Ouroboros.DATATYPE_INT32();
Ouroboros.datatypes["INT64"]		= new Ouroboros.DATATYPE_INT64();

Ouroboros.datatypes["FLOAT"]		= new Ouroboros.DATATYPE_FLOAT();
Ouroboros.datatypes["DOUBLE"]	= new Ouroboros.DATATYPE_DOUBLE();

Ouroboros.datatypes["STRING"]	= new Ouroboros.DATATYPE_STRING();
Ouroboros.datatypes["VECTOR2"]	= new Ouroboros.DATATYPE_VECTOR2;
Ouroboros.datatypes["VECTOR3"]	= new Ouroboros.DATATYPE_VECTOR3;
Ouroboros.datatypes["VECTOR4"]	= new Ouroboros.DATATYPE_VECTOR4;
Ouroboros.datatypes["PYTHON"]	= new Ouroboros.DATATYPE_PYTHON();
Ouroboros.datatypes["UNICODE"]	= new Ouroboros.DATATYPE_UNICODE();
Ouroboros.datatypes["ENTITYCALL"]= new Ouroboros.DATATYPE_ENTITYCALL();
Ouroboros.datatypes["BLOB"]		= new Ouroboros.DATATYPE_BLOB();

/*-----------------------------------------------------------------------------------------
												Ouroboros args
-----------------------------------------------------------------------------------------*/
Ouroboros.OuroborosArgs = function()
{
	this.ip = "127.0.0.1";
	this.port = @{OURO_USE_ALIAS_ENTITYID};
	this.updateHZ = @{OURO_UPDATEHZ} * 10;
	this.serverHeartbeatTick = @{OURO_SERVER_EXTERNAL_TIMEOUT};

	// Reference: http://www.ouroboros.org/docs/programming/clientsdkprogramming.html, client types
	this.clientType = 5;

	// 在Entity�?始化时是�?�触�?�属性的set_*事件(callPropertysSetMethods)
	this.isOnInitCallPropertysSetMethods = true;

	// 是�?�用wss, 默认使用ws
	this.isWss = false;
}

/*-----------------------------------------------------------------------------------------
												Ouroboros app
-----------------------------------------------------------------------------------------*/
Ouroboros.EventTypes =
{
	// Create new account.
	// <para> param1(string): accountName</para>
	// <para> param2(string): password</para>
	// <para> param3(bytes): datas // Datas by user defined. Data will be recorded into the KBE account database, you can access the datas through the script layer. If you use third-party account system, datas will be submitted to the third-party system.</para>
	createAccount : "createAccount",

	// Login to server.
	// <para> param1(string): accountName</para>
	// <para> param2(string): password</para>
	// <para> param3(bytes): datas // Datas by user defined. Data will be recorded into the KBE account database, you can access the datas through the script layer. If you use third-party account system, datas will be submitted to the third-party system.</para>
	login : "login",

	// Logout to baseapp, called when exiting the client.	
	logout : "logout",

	// Relogin to baseapp.
	reloginBaseapp : "reloginBaseapp",

	// Request server binding account Email.
	// <para> param1(string): emailAddress</para>
	bindAccountEmail : "bindAccountEmail",

	// Request to set up a new password for the account. Note: account must be online.
	// <para> param1(string): old_password</para>
	// <para> param2(string): new_password</para>
	newPassword : "newPassword",

	// ------------------------------------连接相关------------------------------------

	// Kicked of the current server.
	// <para> param1(uint16): retcode. // server_errors</para>
	onKicked : "onKicked",

	// Disconnected from the server.
	onDisconnected : "onDisconnected",

	// Status of connection server.
	// <para> param1(bool): success or fail</para>
	onConnectionState : "onConnectionState",

	// ------------------------------------logon相关------------------------------------

	// Create account feedback results.
	// <para> param1(uint16): retcode. // server_errors</para>
	// <para> param2(bytes): datas. // If you use third-party account system, the system may fill some of the third-party additional datas. </para>
	onCreateAccountResult : "onCreateAccountResult",

	// Engine version mismatch.
	// <para> param1(string): clientVersion
	// <para> param2(string): serverVersion
	onVersionNotMatch : "onVersionNotMatch",

	// script version mismatch.
    // <para> param1(string): clientScriptVersion
    // <para> param2(string): serverScriptVersion
	onScriptVersionNotMatch : "onScriptVersionNotMatch",

	// Login failed.
    // <para> param1(uint16): retcode. // server_errors</para>
	onLoginFailed : "onLoginFailed",

	// Login to baseapp.
	onLoginBaseapp : "onLoginBaseapp",

	// Login baseapp failed.
    // <para> param1(uint16): retcode. // server_errors</para>
	onLoginBaseappFailed : "onLoginBaseappFailed",

	// Relogin to baseapp.
	onReloginBaseapp : "onReloginBaseapp",

	// Relogin baseapp success.
	onReloginBaseappSuccessfully : "onReloginBaseappSuccessfully",

	// Relogin baseapp failed.
    // <para> param1(uint16): retcode. // server_errors</para>
	onReloginBaseappFailed : "onReloginBaseappFailed",

	// ------------------------------------实体cell相关事件------------------------------------

	// Entity enter the client-world.
    // <para> param1: Entity</para>
	onEnterWorld : "onEnterWorld",

	// Entity leave the client-world.
    // <para> param1: Entity</para>
	onLeaveWorld : "onLeaveWorld",

	// Player enter the new space.
    // <para> param1: Entity</para>
	onEnterSpace : "onEnterSpace",

	// Player leave the space.
    // <para> param1: Entity</para>
	onLeaveSpace : "onLeaveSpace",

	// Sets the current position of the entity.
	// <para> param1: Entity</para>
	set_position : "set_position",

	// Sets the current direction of the entity.
	// <para> param1: Entity</para>
	set_direction : "set_direction",

	// The entity position is updated, you can smooth the moving entity to new location.
	// <para> param1: Entity</para>
	updatePosition : "updatePosition",

	// The current space is specified by the geometry mapping.
	// Popular said is to load the specified Map Resources.
	// <para> param1(string): resPath</para>
	addSpaceGeometryMapping : "addSpaceGeometryMapping",

	// Server spaceData set data.
	// <para> param1(int32): spaceID</para>
	// <para> param2(string): key</para>
	// <para> param3(string): value</para>
	onSetSpaceData : "onSetSpaceData",

	// Start downloading data.
	// <para> param1(int32): rspaceID</para>
	// <para> param2(string): key</para>
	onDelSpaceData : "onDelSpaceData",

	// Triggered when the entity is controlled or out of control.
	// <para> param1: Entity</para>
	// <para> param2(bool): isControlled</para>
	onControlled : "onControlled",

	// Lose controlled entity.
	// <para> param1: Entity</para>
	onLoseControlledEntity : "onLoseControlledEntity",

	// ------------------------------------数�?�下载相关------------------------------------

	// Start downloading data.
	// <para> param1(uint16): resouce id</para>
	// <para> param2(uint32): data size</para>
	// <para> param3(string): description</para>
	onStreamDataStarted : "onStreamDataStarted",

	// Receive data.
	// <para> param1(uint16): resouce id</para>
	// <para> param2(bytes): datas</para>
	onStreamDataRecv : "onStreamDataRecv",

	// The downloaded data is completed.
	// <para> param1(uint16): resouce id</para>
	onStreamDataCompleted : "onStreamDataCompleted",
}

Ouroboros.OuroborosApp = function(ouroborosArgs)
{
	console.assert(Ouroboros.app == null || Ouroboros.app == undefined, "Assertion of Ouroboros.app not is null");
	
	Ouroboros.app = this;
	
	this.args = ouroborosArgs;
	
	this.username = "testhtml51";
	this.password = "123456";
	this.clientdatas = "";
	this.encryptedKey = "";
	
	this.loginappMessageImported = false;
	this.baseappMessageImported = false;
	this.serverErrorsDescrImported = false;
	this.entitydefImported = false;
	
	Ouroboros.getSingleton = function()
	{
		console.assert(Ouroboros.app != undefined, "OuroborosApp is null");
		return Ouroboros.app;
	}

	// �??述�?务端返回的错误信�?�
	Ouroboros.ServerErr = function()
	{
		this.name = "";
		this.descr = "";
		this.id = 0;
	}

	this.serverErrs = {};
		
	// 登录loginapp的地�?�
	this.ip = this.args.ip;
	this.port = this.args.port;
	this.isWss = this.args.isWss;
	this.protocol = this.isWss ? "wss://" : "ws://";
	
	// �?务端分�?的baseapp地�?�
	this.baseappIP = "";
	this.baseappTcpPort = 0;
	this.baseappUdpPort = 0;
	
	this.currMsgID = 0;
	this.currMsgCount = 0;
	this.currMsgLen = 0;
	
	Ouroboros.FragmentDataTypes = 
	{
		FRAGMENT_DATA_UNKNOW : 0,
		FRAGMENT_DATA_MESSAGE_ID : 1,
		FRAGMENT_DATA_MESSAGE_LENGTH : 2,
		FRAGMENT_DATA_MESSAGE_LENGTH1 : 3,
		FRAGMENT_DATA_MESSAGE_BODY : 4
	};

	this.fragmentStream = null;
	this.fragmentDatasFlag = Ouroboros.FragmentDataTypes.FRAGMENT_DATA_UNKNOW;
	this.fragmentDatasRemain = 0;

	this.msgStream = new Ouroboros.MemoryStream(Ouroboros.PACKET_MAX_SIZE_TCP);

	this.resetSocket = function()
	{
		try
		{  
			if(Ouroboros.app.socket != undefined && Ouroboros.app.socket != null)
			{
				var sock = Ouroboros.app.socket;
				
				sock.onopen = undefined;
				sock.onerror = undefined;
				sock.onmessage = undefined;
				sock.onclose = undefined;
				Ouroboros.app.socket = null;
				sock.close();
			}
		}
		catch(e)
		{ 
		}
	}
	
	this.reset = function()
	{
		if(Ouroboros.app.entities != undefined && Ouroboros.app.entities != null)
		{
			Ouroboros.app.clearEntities(true);
		}
		
		Ouroboros.app.resetSocket();
		
		Ouroboros.app.currserver = "loginapp";
		Ouroboros.app.currstate = "create";
		Ouroboros.app.currconnect = "loginapp";

		// 扩展数�?�
		Ouroboros.app.serverdatas = "";
		
		// 版本信�?�
		Ouroboros.app.serverVersion = "";
		Ouroboros.app.serverScriptVersion = "";
		Ouroboros.app.serverProtocolMD5 = "@{OURO_SERVER_PROTO_MD5}";
		Ouroboros.app.serverEntityDefMD5 = "@{OURO_SERVER_ENTITYDEF_MD5}";
		Ouroboros.app.clientVersion = "@{OURO_VERSION}";
		Ouroboros.app.clientScriptVersion = "@{OURO_SCRIPT_VERSION}";
		
		// player的相关信�?�
		Ouroboros.app.entity_uuid = null;
		Ouroboros.app.entity_id = 0;
		Ouroboros.app.entity_type = "";

		// 这个�?�数的选择必须与ouroboros_defs.xml::cellapp/aliasEntityID的�?�数�?�?一致
		Ouroboros.app.useAliasEntityID = @{OURO_USE_ALIAS_ENTITYID};

		// 当�?玩家最�?�一次�?�步到�?务端的�?置与�?�?�与�?务端最�?�一次�?�步过�?�的�?置
		Ouroboros.app.entityServerPos = new Ouroboros.Vector3(0.0, 0.0, 0.0);
		
		// 客户端所有的实体
		Ouroboros.app.entities = {};
		Ouroboros.app.entityIDAliasIDList = [];
		Ouroboros.app.controlledEntities = [];

		// 空间的信�?�
		Ouroboros.app.spacedata = {};
		Ouroboros.app.spaceID = 0;
		Ouroboros.app.spaceResPath = "";
		Ouroboros.app.isLoadedGeometry = false;
		
		var dateObject = new Date();
		Ouroboros.app.lastTickTime = dateObject.getTime();
		Ouroboros.app.lastTickCBTime = dateObject.getTime();
		
		Ouroboros.mappingDataType();
		
		// 当�?组件类别， �?套�?务端体系
		Ouroboros.app.component = "client";
	}

	this.installEvents = function()
	{
		Ouroboros.Event.register(Ouroboros.EventTypes.createAccount, Ouroboros.app, "createAccount");
		Ouroboros.Event.register(Ouroboros.EventTypes.login, Ouroboros.app, "login");
		Ouroboros.Event.register(Ouroboros.EventTypes.logout, Ouroboros.app, "logout");
		Ouroboros.Event.register(Ouroboros.EventTypes.reloginBaseapp, Ouroboros.app, "reloginBaseapp");
		Ouroboros.Event.register(Ouroboros.EventTypes.bindAccountEmail, Ouroboros.app, "bindAccountEmail");
		Ouroboros.Event.register(Ouroboros.EventTypes.newPassword, Ouroboros.app, "newPassword");
	}

	this.uninstallEvents = function()
	{
		Ouroboros.Event.deregister(Ouroboros.EventTypes.createAccount, Ouroboros.app);
		Ouroboros.Event.deregister(Ouroboros.EventTypes.login, Ouroboros.app);
		Ouroboros.Event.deregister(Ouroboros.EventTypes.logout, Ouroboros.app);
		Ouroboros.Event.deregister(Ouroboros.EventTypes.reloginBaseapp, Ouroboros.app);
		Ouroboros.Event.deregister(Ouroboros.EventTypes.bindAccountEmail, Ouroboros.app);
		Ouroboros.Event.deregister(Ouroboros.EventTypes.newPassword, Ouroboros.app);
	}
	
	this.hello = function()
	{  
		var bundle = Ouroboros.Bundle.createObject();
		
		if(Ouroboros.app.currserver == "loginapp")
			bundle.newMessage(Ouroboros.messages.Loginapp_hello);
		else
			bundle.newMessage(Ouroboros.messages.Baseapp_hello);
		
		bundle.writeString(Ouroboros.app.clientVersion);
		bundle.writeString(Ouroboros.app.clientScriptVersion);
		bundle.writeBlob(Ouroboros.app.encryptedKey);
		bundle.send(Ouroboros.app);
	}

	this.player = function()
	{
		return Ouroboros.app.entities[Ouroboros.app.entity_id];
	}

	this.findEntity = function(entityID)
	{
		return Ouroboros.app.entities[entityID];
	}
		
	this.connect = function(addr)
	{
		console.assert(Ouroboros.app.socket == null, "Assertion of socket not is null");
		
		try
		{  
			Ouroboros.app.socket = new WebSocket(addr);  
		}
		catch(e)
		{  
			Ouroboros.ERROR_MSG('WebSocket init error(' + e.toString() + ')!'); 
			Ouroboros.Event.fire(Ouroboros.EventTypes.onConnectionState, false);
			return;  
		}
		
		Ouroboros.app.socket.binaryType = "arraybuffer";
		Ouroboros.app.socket.onopen = Ouroboros.app.onopen;  
		Ouroboros.app.socket.onerror = Ouroboros.app.onerror_before_onopen;  
		Ouroboros.app.socket.onmessage = Ouroboros.app.onmessage;  
		Ouroboros.app.socket.onclose = Ouroboros.app.onclose;
	}

	this.disconnect = function()
	{
		Ouroboros.app.resetSocket();
	}
	
	this.onopen = function()
	{  
		Ouroboros.INFO_MSG('connect success!');
		Ouroboros.app.socket.onerror = Ouroboros.app.onerror_after_onopen;
		Ouroboros.Event.fire(Ouroboros.EventTypes.onConnectionState, true);
	}

	this.onerror_before_onopen = function(evt)
	{  
		Ouroboros.ERROR_MSG('onerror_before_onopen error:' + evt.data);
		Ouroboros.app.resetSocket();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onConnectionState, false);
	}
	
	this.onerror_after_onopen = function(evt)
	{
		Ouroboros.ERROR_MSG('onerror_after_onopen error:' + evt.data);
		Ouroboros.app.resetSocket();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onDisconnected);
	}
	
	this.onmessage = function(msg)
	{ 
		var stream = Ouroboros.app.msgStream;
		stream.setbuffer(msg.data);
		stream.wpos = msg.data.byteLength;

		var app =  Ouroboros.app;
		var FragmentDataTypes = Ouroboros.FragmentDataTypes;

		while(stream.length() > 0 || app.fragmentStream != null)
		{
			if(app.fragmentDatasFlag == FragmentDataTypes.FRAGMENT_DATA_UNKNOW)
			{
				if(app.currMsgID == 0)
				{
					if(Ouroboros.MESSAGE_ID_LENGTH > 1 && stream.length() < Ouroboros.MESSAGE_ID_LENGTH)
					{
						app.writeFragmentMessage(FragmentDataTypes.FRAGMENT_DATA_MESSAGE_ID, stream, Ouroboros.MESSAGE_ID_LENGTH);
						break;
					}

					app.currMsgID = stream.readUint16();
				}
					
				var msgHandler = Ouroboros.clientmessages[app.currMsgID];
				
				if(!msgHandler)
				{
					app.currMsgID = 0;
					app.currMsgLen = 0;
					Ouroboros.ERROR_MSG("OuroborosApp::onmessage[" + app.currserver + "]: not found msg(" + app.currMsgID + ")!");
					break;
				}

				if(app.currMsgLen == 0)
				{
					var msglen = msgHandler.length;
					if(msglen == -1)
					{
						if(stream.length() < Ouroboros.MESSAGE_LENGTH_LENGTH)
						{
							app.writeFragmentMessage(FragmentDataTypes.FRAGMENT_DATA_MESSAGE_LENGTH, stream, Ouroboros.MESSAGE_LENGTH_LENGTH);
							break;
						}
						else
						{
							msglen = stream.readUint16();
							app.currMsgLen = msglen;

							// 扩展长度
							if(msglen == Ouroboros.MESSAGE_MAX_SIZE)
							{
								if(stream.length() < Ouroboros.MESSAGE_LENGTH1_LENGTH)
								{
									app.writeFragmentMessage(FragmentDataTypes.FRAGMENT_DATA_MESSAGE_LENGTH1, stream, Ouroboros.MESSAGE_LENGTH1_LENGTH);
									break;
								}

								app.currMsgLen = stream.readUint32();
							}
						}
					}
					else
					{
						app.currMsgLen = msglen;
					}
				}

				if(app.fragmentStream != null && app.fragmentStream.length() >= app.currMsgLen)
				{
					msgHandler.handleMessage(app.fragmentStream);
					app.fragmentStream = null;
				}
				else if(stream.length() < app.currMsgLen && stream.length() > 0)
				{
					app.writeFragmentMessage(FragmentDataTypes.FRAGMENT_DATA_MESSAGE_BODY, stream, app.currMsgLen);
					break;
				}
				else
				{
					var wpos = stream.wpos;
					var rpos = stream.rpos + msglen;
					stream.wpos = rpos;
					msgHandler.handleMessage(stream);
					stream.wpos = wpos;
					stream.rpos = rpos;
				}

				app.currMsgID = 0;
				app.currMsgLen = 0;
				app.fragmentStream = null;
			}
			else
			{
				if(app.mergeFragmentMessage(stream))
					break;
			}
		}
	}  

	this.writeFragmentMessage = function(FragmentDataType, stream, datasize)
	{
		if(!(stream instanceof Ouroboros.MemoryStream))
		{
			Ouroboros.ERROR_MSG("writeFragmentMessage(): stream must be MemoryStream instances!");
			return;
		}

		var app = Ouroboros.app;
		var opsize = stream.length();
		
		app.fragmentDatasRemain = datasize - opsize;
		app.fragmentDatasFlag = FragmentDataType;
		app.fragmentStream = stream;
	}

	this.mergeFragmentMessage = function(stream)
	{
		if(!(stream instanceof Ouroboros.MemoryStream))
		{
			Ouroboros.ERROR_MSG("mergeFragmentMessage(): stream must be MemoryStream instances!");
			return false;
		}

		var opsize = stream.length();
		if(opsize == 0)
			return false;

		var app = Ouroboros.app;
		var fragmentStream = app.fragmentStream;
		console.assert(fragmentStream != null);

		if(opsize >= app.fragmentDatasRemain)
		{
			var FragmentDataTypes = Ouroboros.FragmentDataTypes;
			fragmentStream.append(stream, stream.rpos, app.fragmentDatasRemain);

			switch(app.fragmentDatasFlag)
			{
				case FragmentDataTypes.FRAGMENT_DATA_MESSAGE_ID:
					app.currMsgID = fragmentStream.readUint16();
					app.fragmentStream = null;
					break;

				case FragmentDataTypes.FRAGMENT_DATA_MESSAGE_LENGTH:
					app.currMsgLen = fragmentStream.readUint16();
					app.fragmentStream = null;
					break;

				case FragmentDataTypes.FRAGMENT_DATA_MESSAGE_LENGTH1:
					app.currMsgLen = fragmentStream.readUint32();
					app.fragmentStream = null;
					break;

				case FragmentDataTypes.FRAGMENT_DATA_MESSAGE_BODY:
				default:
					break;
			}

			stream.rpos += app.fragmentDatasRemain;
			app.fragmentDatasFlag = FragmentDataTypes.FRAGMENT_DATA_UNKNOW;
			app.fragmentDatasRemain = 0;
			return false;
		}
		else
		{
			fragmentStream.append(stream, stream.rpos, opsize);
			app.fragmentDatasRemain -= opsize;
			stream.done();
			return true;
		}
	}

	this.onclose = function()
	{  
		Ouroboros.INFO_MSG('connect close:' + Ouroboros.app.currserver);

		if(Ouroboros.app.currconnect != Ouroboros.app.currserver)
			return;

		Ouroboros.app.resetSocket();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onDisconnected);
		//if(Ouroboros.app.currserver != "loginapp")
		//	Ouroboros.app.reset();
	}

	this.send = function(msg)
	{
		Ouroboros.app.socket.send(msg);
	}

	this.close = function() {
		Ouroboros.INFO_MSG('Ouroboros::close()');
		Ouroboros.app.socket.close();  
		Ouroboros.app.reset();
	}
	
	this.update = function()
	{
		if(Ouroboros.app.socket == null)
			return;

		var dateObject = new Date();
		if(Ouroboros.app.args.serverHeartbeatTick > 0 && (dateObject.getTime() - Ouroboros.app.lastTickTime) / 1000 > (Ouroboros.app.args.serverHeartbeatTick / 2))
		{
			// 如果心跳回调接收时间�?于心跳�?��?时间，说明没有收到回调
			// 此时应该通知客户端掉线了
			if(Ouroboros.app.lastTickCBTime < Ouroboros.app.lastTickTime)
			{
				Ouroboros.ERROR_MSG("sendTick: Receive appTick timeout!");
				Ouroboros.app.socket.close();  
			}
			
			if(Ouroboros.app.currserver == "loginapp")
			{
				if(Ouroboros.messages.Loginapp_onClientActiveTick != undefined)
				{
					var bundle = Ouroboros.Bundle.createObject();
					bundle.newMessage(Ouroboros.messages.Loginapp_onClientActiveTick);
					bundle.send(Ouroboros.app);
				}
			}
			else
			{
				if(Ouroboros.messages.Baseapp_onClientActiveTick != undefined)
				{
					var bundle = Ouroboros.Bundle.createObject();
					bundle.newMessage(Ouroboros.messages.Baseapp_onClientActiveTick);
					bundle.send(Ouroboros.app);
				}
			}
			
			Ouroboros.app.lastTickTime = dateObject.getTime();
		}
		
		Ouroboros.app.updatePlayerToServer();
	}

	/*
		�?务器心跳回调
	*/
	this.Client_onAppActiveTickCB = function()
	{
		var dateObject = new Date();
		Ouroboros.app.lastTickCBTime = dateObject.getTime();
	}

	/*
		通过错误id得到错误�??述
	*/
	this.serverErr = function(id)
	{
		var e = Ouroboros.app.serverErrs[id];
		
		if(e == undefined)
		{
			return "";
		}

		return e.name + " [" + e.descr + "]";
	}

	/*
		�?务端错误�??述导入了
	*/
	this.Client_onImportServerErrorsDescr = function(stream)
	{
		var size = stream.readUint16();
		while(size > 0)
		{
			size -= 1;
			
			var e = new Ouroboros.ServerErr();
			e.id = stream.readUint16();
			e.name = Ouroboros.utf8ArrayToString(stream.readBlob());
			e.descr = Ouroboros.utf8ArrayToString(stream.readBlob());
			
			Ouroboros.app.serverErrs[e.id] = e;
				
			Ouroboros.INFO_MSG("Client_onImportServerErrorsDescr: id=" + e.id + ", name=" + e.name + ", descr=" + e.descr);
		}
	}

	this.Client_onImportClientSdk = function(stream)
	{
		var remainingFiles = stream.readInt32();
		var fileName = stream.readString();
		var fileSize = stream.readInt32();
		var fileDatas = stream.readBlob()
		Ouroboros.Event.fire("onImportClientSDK", remainingFiles, fileName, fileSize, fileDatas);
	}

	this.onOpenLoginapp_login = function()
	{  
		Ouroboros.INFO_MSG("OuroborosApp::onOpenLoginapp_login: successfully!");
		Ouroboros.Event.fire(Ouroboros.EventTypes.onConnectionState, true);
		
		Ouroboros.app.currserver = "loginapp";
		Ouroboros.app.currstate = "login";
		
		if(!Ouroboros.app.loginappMessageImported)
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Loginapp_importClientMessages);
			bundle.send(Ouroboros.app);
			Ouroboros.app.socket.onmessage = Ouroboros.app.Client_onImportClientMessages;  
			Ouroboros.INFO_MSG("OuroborosApp::onOpenLoginapp_login: start importClientMessages ...");
			Ouroboros.Event.fire("Loginapp_importClientMessages");
		}
		else
		{
			Ouroboros.app.onImportClientMessagesCompleted();
		}
	}
	
	this.onOpenLoginapp_createAccount = function()
	{  
		Ouroboros.Event.fire(Ouroboros.EventTypes.onConnectionState, true);
		Ouroboros.INFO_MSG("OuroborosApp::onOpenLoginapp_createAccount: successfully!");
		Ouroboros.app.currserver = "loginapp";
		Ouroboros.app.currstate = "createAccount";
		
		if(!Ouroboros.app.loginappMessageImported)
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Loginapp_importClientMessages);
			bundle.send(Ouroboros.app);
			Ouroboros.app.socket.onmessage = Ouroboros.app.Client_onImportClientMessages;  
			Ouroboros.INFO_MSG("OuroborosApp::onOpenLoginapp_createAccount: start importClientMessages ...");
			Ouroboros.Event.fire("Loginapp_importClientMessages");
		}
		else
		{
			Ouroboros.app.onImportClientMessagesCompleted();
		}
	}
	
	this.onImportClientMessagesCompleted = function()
	{
		Ouroboros.INFO_MSG("OuroborosApp::onImportClientMessagesCompleted: successfully!");
		Ouroboros.app.socket.onmessage = Ouroboros.app.onmessage; 
		Ouroboros.app.hello();
		
		if(Ouroboros.app.currserver == "loginapp")
		{
			if(!Ouroboros.app.serverErrorsDescrImported)
			{
				Ouroboros.INFO_MSG("Ouroboros::onImportClientMessagesCompleted(): send importServerErrorsDescr!");
				Ouroboros.app.serverErrorsDescrImported = true;
				var bundle = Ouroboros.Bundle.createObject();
				bundle.newMessage(Ouroboros.messages.Loginapp_importServerErrorsDescr);
				bundle.send(Ouroboros.app);
			}
							
			if(Ouroboros.app.currstate == "login")
				Ouroboros.app.login_loginapp(false);
			else if(Ouroboros.app.currstate == "resetpassword")
				Ouroboros.app.resetpassword_loginapp(false);
			else
				Ouroboros.app.createAccount_loginapp(false);
			
			Ouroboros.app.loginappMessageImported = true;
		}
		else
		{
			Ouroboros.app.baseappMessageImported = true;
			
			if(!Ouroboros.app.entitydefImported)
			{
				Ouroboros.INFO_MSG("OuroborosApp::onImportClientMessagesCompleted: start importEntityDef ...");
				var bundle = Ouroboros.Bundle.createObject();
				bundle.newMessage(Ouroboros.messages.Baseapp_importClientEntityDef);
				bundle.send(Ouroboros.app);
				Ouroboros.Event.fire("Baseapp_importClientEntityDef");
			}
			else
			{
				Ouroboros.app.onImportEntityDefCompleted();
			}
		}
	}

	this.createDataTypeFromStreams = function(stream, canprint)
	{
		var aliassize = stream.readUint16();
		Ouroboros.INFO_MSG("OuroborosApp::createDataTypeFromStreams: importAlias(size=" + aliassize + ")!");
		
		while(aliassize > 0)
		{
			aliassize--;
			Ouroboros.app.createDataTypeFromStream(stream, canprint);
		};	
		
		for(var datatype in Ouroboros.datatypes)
		{
			if(Ouroboros.datatypes[datatype] != undefined)
			{
				Ouroboros.datatypes[datatype].bind();
			}
		}
	}

	this.createDataTypeFromStream = function(stream, canprint)
	{
		var utype = stream.readUint16();
		var name = stream.readString();
		var valname = stream.readString();
		
		/* 有一些匿�??类型，我们需�?�??供一个唯一�??称放到datatypes中
			如：
			<onRemoveAvatar>
				<Arg>	ARRAY <of> INT8 </of>		</Arg>
			</onRemoveAvatar>				
		*/
		if(valname.length == 0)
			valname = "Null_" + utype;
			
		if(canprint)
			Ouroboros.INFO_MSG("OuroborosApp::Client_onImportClientEntityDef: importAlias(" + name + ":" + valname + ")!");
		
		if(name == "FIXED_DICT")
		{
			var datatype = new Ouroboros.DATATYPE_FIXED_DICT();
			var keysize = stream.readUint8();
			datatype.implementedBy = stream.readString();
				
			while(keysize > 0)
			{
				keysize--;
				
				var keyname = stream.readString();
				var keyutype = stream.readUint16();
				datatype.dicttype[keyname] = keyutype;
			};
			
			Ouroboros.datatypes[valname] = datatype;
		}
		else if(name == "ARRAY")
		{
			var uitemtype = stream.readUint16();
			var datatype = new Ouroboros.DATATYPE_ARRAY();
			datatype.type = uitemtype;
			Ouroboros.datatypes[valname] = datatype;
		}
		else
		{
			Ouroboros.datatypes[valname] = Ouroboros.datatypes[name];
		}

		Ouroboros.datatypes[utype] = Ouroboros.datatypes[valname];
		
		// 将用户自定义的类型补充到映射表中
		Ouroboros.datatype2id[valname] = utype;
	}
	
	this.Client_onImportClientEntityDef = function(stream)
	{
		Ouroboros.app.createDataTypeFromStreams(stream, true);
		
		while(stream.length() > 0)
		{
			var scriptmodule_name = stream.readString();
			var scriptUtype = stream.readUint16();
			var propertysize = stream.readUint16();
			var methodsize = stream.readUint16();
			var base_methodsize = stream.readUint16();
			var cell_methodsize = stream.readUint16();
			
			Ouroboros.INFO_MSG("OuroborosApp::Client_onImportClientEntityDef: import(" + scriptmodule_name + "), propertys(" + propertysize + "), " +
					"clientMethods(" + methodsize + "), baseMethods(" + base_methodsize + "), cellMethods(" + cell_methodsize + ")!");
			
			Ouroboros.moduledefs[scriptmodule_name] = {};
			var currModuleDefs = Ouroboros.moduledefs[scriptmodule_name];
			currModuleDefs["name"] = scriptmodule_name;
			currModuleDefs["propertys"] = {};
			currModuleDefs["methods"] = {};
			currModuleDefs["base_methods"] = {};
			currModuleDefs["cell_methods"] = {};
			Ouroboros.moduledefs[scriptUtype] = currModuleDefs;
			
			var self_propertys = currModuleDefs["propertys"];
			var self_methods = currModuleDefs["methods"];
			var self_base_methods = currModuleDefs["base_methods"];
			var self_cell_methods = currModuleDefs["cell_methods"];
			
			var Class = Ouroboros[scriptmodule_name];
			
			while(propertysize > 0)
			{
				propertysize--;
				
				var properUtype = stream.readUint16();
				var properFlags = stream.readUint32();
				var aliasID = stream.readInt16();
				var name = stream.readString();
				var defaultValStr = stream.readString();
				var utype = Ouroboros.datatypes[stream.readUint16()];
				var setmethod = null;
				
				if(Class != undefined)
				{
					setmethod = Class.prototype["set_" + name];
					if(setmethod == undefined)
						setmethod = null;
				}
				
				var savedata = [properUtype, aliasID, name, defaultValStr, utype, setmethod, properFlags];
				self_propertys[name] = savedata;
				
				if(aliasID != -1)
				{
					self_propertys[aliasID] = savedata;
					currModuleDefs["usePropertyDescrAlias"] = true;
				}
				else
				{
					self_propertys[properUtype] = savedata;
					currModuleDefs["usePropertyDescrAlias"] = false;
				}
				
				Ouroboros.INFO_MSG("OuroborosApp::Client_onImportClientEntityDef: add(" + scriptmodule_name + "), property(" + name + "/" + properUtype + ").");
			};
			
			while(methodsize > 0)
			{
				methodsize--;
				
				var methodUtype = stream.readUint16();
				var aliasID = stream.readInt16();
				var name = stream.readString();
				var argssize = stream.readUint8();
				var args = [];
				
				while(argssize > 0)
				{
					argssize--;
					args.push(Ouroboros.datatypes[stream.readUint16()]);
				};
				
				var savedata = [methodUtype, aliasID, name, args];
				self_methods[name] = savedata;
				
				if(aliasID != -1)
				{
					self_methods[aliasID] = savedata;
					currModuleDefs["useMethodDescrAlias"] = true;
				}
				else
				{
					self_methods[methodUtype] = savedata;
					currModuleDefs["useMethodDescrAlias"] = false;
				}
				
				Ouroboros.INFO_MSG("OuroborosApp::Client_onImportClientEntityDef: add(" + scriptmodule_name + "), method(" + name + ").");
			};

			while(base_methodsize > 0)
			{
				base_methodsize--;
				
				var methodUtype = stream.readUint16();
				var aliasID = stream.readInt16();
				var name = stream.readString();
				var argssize = stream.readUint8();
				var args = [];
				
				while(argssize > 0)
				{
					argssize--;
					args.push(Ouroboros.datatypes[stream.readUint16()]);
				};
				
				self_base_methods[name] = [methodUtype, aliasID, name, args];
				Ouroboros.INFO_MSG("OuroborosApp::Client_onImportClientEntityDef: add(" + scriptmodule_name + "), base_method(" + name + ").");
			};
			
			while(cell_methodsize > 0)
			{
				cell_methodsize--;
				
				var methodUtype = stream.readUint16();
				var aliasID = stream.readInt16();
				var name = stream.readString();
				var argssize = stream.readUint8();
				var args = [];
				
				while(argssize > 0)
				{
					argssize--;
					args.push(Ouroboros.datatypes[stream.readUint16()]);
				};
				
				self_cell_methods[name] = [methodUtype, aliasID, name, args];
				Ouroboros.INFO_MSG("OuroborosApp::Client_onImportClientEntityDef: add(" + scriptmodule_name + "), cell_method(" + name + ").");
			};
			
			var defmethod = Ouroboros[scriptmodule_name];

			if(defmethod == undefined)
			{
				Ouroboros.ERROR_MSG("OuroborosApp::Client_onImportClientEntityDef: module(" + scriptmodule_name + ") not found!");
			}
			
			for(var name in currModuleDefs.propertys)
			{
				var infos = currModuleDefs.propertys[name];
				var properUtype = infos[0];
				var aliasID = infos[1];
				var name = infos[2];
				var defaultValStr = infos[3];
				var utype = infos[4];

				if(defmethod != undefined)
					defmethod.prototype[name] = utype.parseDefaultValStr(defaultValStr);
			};

			for(var name in currModuleDefs.methods)
			{
				var infos = currModuleDefs.methods[name];
				var properUtype = infos[0];
				var aliasID = infos[1];
				var name = infos[2];
				var args = infos[3];
				
				if(defmethod != undefined && defmethod.prototype[name] == undefined)
				{
					Ouroboros.WARNING_MSG(scriptmodule_name + ":: method(" + name + ") no implement!");
				}
			};
		}
		
		Ouroboros.app.onImportEntityDefCompleted();
	}

	this.Client_onVersionNotMatch = function(stream)
	{
		Ouroboros.app.serverVersion = stream.readString();
		Ouroboros.ERROR_MSG("Client_onVersionNotMatch: verInfo=" + Ouroboros.app.clientVersion + " not match(server: " + Ouroboros.app.serverVersion + ")");
		Ouroboros.Event.fire(Ouroboros.EventTypes.onVersionNotMatch, Ouroboros.app.clientVersion, Ouroboros.app.serverVersion);
	}

	this.Client_onScriptVersionNotMatch = function(stream)
	{
		Ouroboros.app.serverScriptVersion = stream.readString();
		Ouroboros.ERROR_MSG("Client_onScriptVersionNotMatch: verInfo=" + Ouroboros.app.clientScriptVersion + " not match(server: " + Ouroboros.app.serverScriptVersion + ")");
		Ouroboros.Event.fire(Ouroboros.EventTypes.onScriptVersionNotMatch, Ouroboros.app.clientScriptVersion, Ouroboros.app.serverScriptVersion);
	}
	
	this.onImportEntityDefCompleted = function()
	{
		Ouroboros.INFO_MSG("OuroborosApp::onImportEntityDefCompleted: successfully!");
		Ouroboros.app.entitydefImported = true;
		Ouroboros.app.login_baseapp(false);
	}

	this.importClientMessages = function(stream)
	{
		var app = Ouroboros.app;

		while(app.currMsgCount > 0)
		{
			app.currMsgCount--;
		
			var msgid = stream.readUint16();
			var msglen = stream.readInt16();
			var msgname = stream.readString();
			var argtype = stream.readInt8();
			var argsize = stream.readUint8();
			var argstypes = new Array(argsize);
			
			for(var i=0; i<argsize; i++)
			{
				argstypes[i] = stream.readUint8();
			}
			
			var handler = null;
			var isClientMethod = msgname.indexOf("Client_") >= 0;
			if(isClientMethod)
			{
				handler = app[msgname];
				if(handler == null || handler == undefined)
				{
					Ouroboros.WARNING_MSG("OuroborosApp::onImportClientMessages[" + app.currserver + "]: interface(" + msgname + "/" + msgid + ") no implement!");
					handler = null;
				}
				else
				{
					Ouroboros.INFO_MSG("OuroborosApp::onImportClientMessages: import(" + msgname + ") successfully!");
				}
			}
	
			if(msgname.length > 0)
			{
				Ouroboros.messages[msgname] = new Ouroboros.Message(msgid, msgname, msglen, argtype, argstypes, handler);
				
				if(isClientMethod)
					Ouroboros.clientmessages[msgid] = Ouroboros.messages[msgname];
				else
					Ouroboros.messages[Ouroboros.app.currserver][msgid] = Ouroboros.messages[msgname];
			}
			else
			{
				Ouroboros.messages[app.currserver][msgid] = new Ouroboros.Message(msgid, msgname, msglen, argtype, argstypes, handler);
			}
		};

		app.onImportClientMessagesCompleted();
		app.currMsgID = 0;
		app.currMsgLen = 0;
		app.currMsgCount = 0;
		app.fragmentStream = null;
	}
	
	this.Client_onImportClientMessages = function(msg)
	{
		var stream = new Ouroboros.MemoryStream(msg.data);
		stream.wpos = msg.data.byteLength;
		var app = Ouroboros.app;

		if(app.currMsgID == 0)
		{
			app.currMsgID = stream.readUint16();
		} 

		if(app.currMsgID == Ouroboros.messages.onImportClientMessages.id)
		{
			if(app.currMsgLen == 0) 
			{
				app.currMsgLen = stream.readUint16();
				app.currMsgCount = stream.readUint16();
			}
			
			var FragmentDataTypes = Ouroboros.FragmentDataTypes
			if(stream.length() + 2 < app.currMsgLen && app.fragmentStream == null)
			{
				app.writeFragmentMessage(FragmentDataTypes.FRAGMENT_DATA_MESSAGE_BODY, stream, app.currMsgLen - 2);
			}
			else if(app.fragmentStream != null)
			{
				app.mergeFragmentMessage(stream);
				
				if(app.fragmentStream.length() + 2 >= app.currMsgLen)
				{
					app.importClientMessages(app.fragmentStream);
				}
			}
			else
			{
				app.importClientMessages(stream);
			}
		}
		else
		{
			Ouroboros.ERROR_MSG("OuroborosApp::onmessage: not found msg(" + app.currMsgID + ")!");
		}
	}
	
	this.createAccount = function(username, password, datas)
	{  
		Ouroboros.app.reset();
		Ouroboros.app.username = username;
		Ouroboros.app.password = password;
		Ouroboros.app.clientdatas = datas;
		
		Ouroboros.app.createAccount_loginapp(true);
	}
	
	this.getServerAddr = function(ip, port)
	{
		var serverAddr = Ouroboros.app.protocol + ip;
		if(port != "")
		{
			serverAddr += ":" + port;
		}
		
		return serverAddr;
	}
	
	this.createAccount_loginapp = function(noconnect)
	{  
		if(noconnect)
		{
			var serverAddr = this.getServerAddr(Ouroboros.app.ip, Ouroboros.app.port);
			Ouroboros.INFO_MSG("OuroborosApp::createAccount_loginapp: start connect to " + serverAddr + "!");
			Ouroboros.app.currconnect = "loginapp";
			Ouroboros.app.connect(serverAddr);
			Ouroboros.app.socket.onopen = Ouroboros.app.onOpenLoginapp_createAccount;  
		}
		else
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Loginapp_reqCreateAccount);
			bundle.writeString(Ouroboros.app.username);
			bundle.writeString(Ouroboros.app.password);
			bundle.writeBlob(Ouroboros.app.clientdatas);
			bundle.send(Ouroboros.app);
		}
	}
	
	this.bindAccountEmail = function(emailAddress)
	{  
		var bundle = Ouroboros.Bundle.createObject();
		bundle.newMessage(Ouroboros.messages.Baseapp_reqAccountBindEmail);
		bundle.writeInt32(Ouroboros.app.entity_id);
		bundle.writeString(Ouroboros.app.password);
		bundle.writeString(emailAddress);
		bundle.send(Ouroboros.app);
	}
	
	this.newPassword = function(old_password, new_password)
	{
		var bundle = Ouroboros.Bundle.createObject();
		bundle.newMessage(Ouroboros.messages.Baseapp_reqAccountNewPassword);
		bundle.writeInt32(Ouroboros.app.entity_id);
		bundle.writeString(old_password);
		bundle.writeString(new_password);
		bundle.send(Ouroboros.app);
	}
	
	this.login = function(username, password, datas)
	{  
		Ouroboros.app.reset();
		Ouroboros.app.username = username;
		Ouroboros.app.password = password;
		Ouroboros.app.clientdatas = datas;
		
		Ouroboros.app.login_loginapp(true);
	}
	
	this.logout = function()
	{
		var bundle = Ouroboros.Bundle.createObject();
		bundle.newMessage(Ouroboros.messages.Baseapp_logoutBaseapp);
		bundle.writeUint64(Ouroboros.app.entity_uuid);
		bundle.writeInt32(Ouroboros.app.entity_id);
		bundle.send(Ouroboros.app);
	}
	
	this.login_loginapp = function(noconnect)
	{  
		if(noconnect)
		{
			var serverAddr = this.getServerAddr(Ouroboros.app.ip, Ouroboros.app.port);
			Ouroboros.INFO_MSG("OuroborosApp::login_loginapp: start connect to " + serverAddr + "!");
			Ouroboros.app.currconnect = "loginapp";
			Ouroboros.app.connect(serverAddr);
			Ouroboros.app.socket.onopen = Ouroboros.app.onOpenLoginapp_login;  
		}
		else
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Loginapp_login);
			bundle.writeInt8(Ouroboros.app.args.clientType); // clientType
			bundle.writeBlob(Ouroboros.app.clientdatas);
			bundle.writeString(Ouroboros.app.username);
			bundle.writeString(Ouroboros.app.password);
			bundle.send(Ouroboros.app);
		}
	}

	this.onOpenLoginapp_resetpassword = function()
	{  
		Ouroboros.INFO_MSG("OuroborosApp::onOpenLoginapp_resetpassword: successfully!");
		Ouroboros.app.currserver = "loginapp";
		Ouroboros.app.currstate = "resetpassword";
		
		if(!Ouroboros.app.loginappMessageImported)
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Loginapp_importClientMessages);
			bundle.send(Ouroboros.app);
			Ouroboros.app.socket.onmessage = Ouroboros.app.Client_onImportClientMessages;  
			Ouroboros.INFO_MSG("OuroborosApp::onOpenLoginapp_resetpassword: start importClientMessages ...");
		}
		else
		{
			Ouroboros.app.onImportClientMessagesCompleted();
		}
	}

	this.reset_password = function(username)
	{ 
		Ouroboros.app.reset();
		Ouroboros.app.username = username;
		Ouroboros.app.resetpassword_loginapp(true);
	}
	
	this.resetpassword_loginapp = function(noconnect)
	{  
		if(noconnect)
		{
			var serverAddr = this.getServerAddr(Ouroboros.app.ip, Ouroboros.app.port);
			Ouroboros.INFO_MSG("OuroborosApp::resetpassword_loginapp: start connect to " + serverAddr + "!");
			Ouroboros.app.currconnect = "loginapp";
			Ouroboros.app.connect(serverAddr);
			Ouroboros.app.socket.onopen = Ouroboros.app.onOpenLoginapp_resetpassword;  
		}
		else
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Loginapp_reqAccountResetPassword);
			bundle.writeString(Ouroboros.app.username);
			bundle.send(Ouroboros.app);
		}
	}
	
	this.onOpenBaseapp = function()
	{
		Ouroboros.INFO_MSG("OuroborosApp::onOpenBaseapp: successfully!");
		Ouroboros.app.currserver = "baseapp";
		
		if(!Ouroboros.app.baseappMessageImported)
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Baseapp_importClientMessages);
			bundle.send(Ouroboros.app);
			Ouroboros.app.socket.onmessage = Ouroboros.app.Client_onImportClientMessages;  
			Ouroboros.Event.fire("Baseapp_importClientMessages");
		}
		else
		{
			Ouroboros.app.onImportClientMessagesCompleted();
		}
	}
	
	this.login_baseapp = function(noconnect)
	{  
		if(noconnect)
		{
			Ouroboros.Event.fire(Ouroboros.EventTypes.onLoginBaseapp);
			var serverAddr = this.getServerAddr(Ouroboros.app.baseappIp, Ouroboros.app.baseappPort);
			Ouroboros.INFO_MSG("OuroborosApp::login_baseapp: start connect to " + serverAddr + "!");
			Ouroboros.app.currconnect = "baseapp";
			Ouroboros.app.connect(serverAddr);
			
			if(Ouroboros.app.socket != undefined && Ouroboros.app.socket != null)
				Ouroboros.app.socket.onopen = Ouroboros.app.onOpenBaseapp;  
		}
		else
		{
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Baseapp_loginBaseapp);
			bundle.writeString(Ouroboros.app.username);
			bundle.writeString(Ouroboros.app.password);
			bundle.send(Ouroboros.app);
		}
	}
	
	this.reloginBaseapp = function()
	{
		var dateObject = new Date();
		Ouroboros.app.lastTickTime = dateObject.getTime();
		Ouroboros.app.lastTickCBTime = dateObject.getTime();

		if(Ouroboros.app.socket != undefined && Ouroboros.app.socket != null)
			return;
		
		Ouroboros.app.resetSocket();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onReloginBaseapp);

		var serverAddr = this.getServerAddr(Ouroboros.app.baseappIp, Ouroboros.app.baseappPort);
		Ouroboros.INFO_MSG("OuroborosApp::reloginBaseapp: start connect to " + serverAddr + "!");
		Ouroboros.app.currconnect = "baseapp";
		Ouroboros.app.connect(serverAddr);
		
		if(Ouroboros.app.socket != undefined && Ouroboros.app.socket != null)
			Ouroboros.app.socket.onopen = Ouroboros.app.onReOpenBaseapp;  
	}
	
	this.onReOpenBaseapp = function()
	{
		Ouroboros.INFO_MSG("OuroborosApp::onReOpenBaseapp: successfully!");
		Ouroboros.app.currserver = "baseapp";
		
		var bundle = Ouroboros.Bundle.createObject();
		bundle.newMessage(Ouroboros.messages.Baseapp_reloginBaseapp);
		bundle.writeString(Ouroboros.app.username);
		bundle.writeString(Ouroboros.app.password);
		bundle.writeUint64(Ouroboros.app.entity_uuid);
		bundle.writeInt32(Ouroboros.app.entity_id);
		bundle.send(Ouroboros.app);
		
		var dateObject = new Date();
		Ouroboros.app.lastTickCBTime = dateObject.getTime();
	}
	
	this.Client_onHelloCB = function(args)
	{
		Ouroboros.app.serverVersion = args.readString();
		Ouroboros.app.serverScriptVersion = args.readString();
		Ouroboros.app.serverProtocolMD5 = args.readString();
		Ouroboros.app.serverEntityDefMD5 = args.readString();
		
		var ctype = args.readInt32();
		
		Ouroboros.INFO_MSG("OuroborosApp::Client_onHelloCB: verInfo(" + Ouroboros.app.serverVersion + "), scriptVerInfo(" + 
			Ouroboros.app.serverScriptVersion + "), serverProtocolMD5(" + Ouroboros.app.serverProtocolMD5 + "), serverEntityDefMD5(" + 
			Ouroboros.app.serverEntityDefMD5 + "), ctype(" + ctype + ")!");
		
		var dateObject = new Date();
		Ouroboros.app.lastTickCBTime = dateObject.getTime();
	}
	
	this.Client_onLoginFailed = function(args)
	{
		var failedcode = args.readUint16();
		Ouroboros.app.serverdatas = args.readBlob();
		Ouroboros.ERROR_MSG("OuroborosApp::Client_onLoginFailed: failedcode=" + failedcode + "(" + Ouroboros.app.serverErrs[failedcode].name + "), datas(" + Ouroboros.app.serverdatas.length + ")!");
		Ouroboros.Event.fire(Ouroboros.EventTypes.onLoginFailed, failedcode, Ouroboros.app.serverdatas);
	}
	
	this.Client_onLoginSuccessfully = function(args)
	{
		var accountName = args.readString();
		Ouroboros.app.username = accountName;
		Ouroboros.app.baseappIp = args.readString();
		Ouroboros.app.baseappTcpPort = args.readUint16();
		Ouroboros.app.baseappUdpPort = args.readUint16();
		Ouroboros.app.serverdatas = args.readBlob();
		
		Ouroboros.INFO_MSG("OuroborosApp::Client_onLoginSuccessfully: accountName(" + accountName + "), addr(" + 
				Ouroboros.app.baseappIp + ":" + Ouroboros.app.baseappTcpPort + ":" + Ouroboros.app.baseappUdpPort + "), datas(" + Ouroboros.app.serverdatas.length + ")!");
		
		Ouroboros.app.disconnect();
		Ouroboros.app.login_baseapp(true);
	}
	
	this.Client_onLoginBaseappFailed = function(failedcode)
	{
		Ouroboros.ERROR_MSG("OuroborosApp::Client_onLoginBaseappFailed: failedcode=" + failedcode + "(" + Ouroboros.app.serverErrs[failedcode].name + ")!");
		Ouroboros.Event.fire(Ouroboros.onLoginBaseappFailed.onLoginBaseappFailed, failedcode);
	}

	this.Client_onReloginBaseappFailed = function(failedcode)
	{
		Ouroboros.ERROR_MSG("OuroborosApp::Client_onReloginBaseappFailed: failedcode="+ failedcode + "(" + Ouroboros.app.serverErrs[failedcode].name + ")!");
		Ouroboros.Event.fire(Ouroboros.EventTypes.onReloginBaseappFailed, failedcode);
	}

	this.Client_onReloginBaseappSuccessfully = function(stream)
	{
		Ouroboros.app.entity_uuid = stream.readUint64();
		Ouroboros.DEBUG_MSG("OuroborosApp::Client_onReloginBaseappSuccessfully: " + Ouroboros.app.username);
		Ouroboros.Event.fire(Ouroboros.EventTypes.onReloginBaseappSuccessfully);
	}
	
	this.entityclass = {};
	this.getentityclass = function(entityType)
	{
		var runclass = Ouroboros.app.entityclass[entityType];
		if(runclass == undefined)
		{
			runclass = Ouroboros[entityType];
			if(runclass == undefined)
			{
				Ouroboros.ERROR_MSG("OuroborosApp::getentityclass: entityType(" + entityType + ") is error!");
				return runclass;
			}
			else
				Ouroboros.app.entityclass[entityType] = runclass;
		}

		return runclass;
	}
	
	this.Client_onCreatedProxies = function(rndUUID, eid, entityType)
	{
		Ouroboros.INFO_MSG("OuroborosApp::Client_onCreatedProxies: eid(" + eid + "), entityType(" + entityType + ")!");
		
		var entity = Ouroboros.app.entities[eid];
		
		Ouroboros.app.entity_uuid = rndUUID;
		Ouroboros.app.entity_id = eid;
		
		if(entity == undefined)
		{
			var runclass = Ouroboros.app.getentityclass(entityType);
			if(runclass == undefined)
				return;
			
			var entity = new runclass();
			entity.id = eid;
			entity.className = entityType;
			
			entity.base = new Ouroboros.EntityCall();
			entity.base.id = eid;
			entity.base.className = entityType;
			entity.base.type = Ouroboros.ENTITYCALL_TYPE_BASE;
			
			Ouroboros.app.entities[eid] = entity;
			
			var entityMessage = Ouroboros.bufferedCreateEntityMessages[eid];
			if(entityMessage != undefined)
			{
				Ouroboros.app.Client_onUpdatePropertys(entityMessage);
				delete Ouroboros.bufferedCreateEntityMessages[eid];
			}
				
			entity.__init__();
			entity.inited = true;
			
			if(Ouroboros.app.args.isOnInitCallPropertysSetMethods)
				entity.callPropertysSetMethods();
		}
		else
		{
			var entityMessage = Ouroboros.bufferedCreateEntityMessages[eid];
			if(entityMessage != undefined)
			{
				Ouroboros.app.Client_onUpdatePropertys(entityMessage);
				delete Ouroboros.bufferedCreateEntityMessages[eid];
			}
		}
	}
	
	this.getViewEntityIDFromStream = function(stream)
	{
		var id = 0;
		if(Ouroboros.app.entityIDAliasIDList.Length > 255)
		{
			id = stream.readInt32();
		}
		else
		{
			var aliasID = stream.readUint8();

			// 如果为0且客户端上一步是�?登陆或者�?连�?作并且�?务端entity在断线期间一直处于在线状�?
			// 则�?�以忽略这个错误, 因为cellapp�?�能一直在�?�baseapp�?��?�?�步消�?�， 当客户端�?连上时未等
			// �?务端�?始化步骤开始则收到�?�步信�?�, 此时这里就会出错。
			if(Ouroboros.app.entityIDAliasIDList.length <= aliasID)
				return 0;
		
			id = Ouroboros.app.entityIDAliasIDList[aliasID];
		}
		
		return id;
	}
	
	this.onUpdatePropertys_ = function(eid, stream)
	{
		var entity = Ouroboros.app.entities[eid];
		
		if(entity == undefined)
		{
			var entityMessage = Ouroboros.bufferedCreateEntityMessages[eid];
			if(entityMessage != undefined)
			{
				Ouroboros.ERROR_MSG("OuroborosApp::Client_onUpdatePropertys: entity(" + eid + ") not found!");
				return;
			}
			
			var stream1 = new Ouroboros.MemoryStream(stream.buffer);
			stream1.wpos = stream.wpos;
			stream1.rpos = stream.rpos - 4;
			Ouroboros.bufferedCreateEntityMessages[eid] = stream1;
			return;
		}
		
		var currModule = Ouroboros.moduledefs[entity.className];
		var pdatas = currModule.propertys;
		while(stream.length() > 0)
		{
			var utype = 0;
			if(currModule.usePropertyDescrAlias)
				utype = stream.readUint8();
			else
				utype = stream.readUint16();
		
			var propertydata = pdatas[utype];
			var setmethod = propertydata[5];
			var flags = propertydata[6];
			var val = propertydata[4].createFromStream(stream);
			var oldval = entity[propertydata[2]];
			
			Ouroboros.INFO_MSG("OuroborosApp::Client_onUpdatePropertys: " + entity.className + "(id=" + eid  + " " + propertydata[2] + ", val=" + val + ")!");
			
			entity[propertydata[2]] = val;
			if(setmethod != null)
			{
				// base类属性或者进入世界�?�cell类属性会触�?�set_*方法
				if(flags == 0x00000020 || flags == 0x00000040)
				{
					if(entity.inited)
						setmethod.call(entity, oldval);
				}
				else
				{
					if(entity.inWorld)
						setmethod.call(entity, oldval);
				}
			}
		}
	}

	this.Client_onUpdatePropertysOptimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		Ouroboros.app.onUpdatePropertys_(eid, stream);
	}
	
	this.Client_onUpdatePropertys = function(stream)
	{
		var eid = stream.readInt32();
		Ouroboros.app.onUpdatePropertys_(eid, stream);
	}

	this.onRemoteMethodCall_ = function(eid, stream)
	{
		var entity = Ouroboros.app.entities[eid];
		
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onRemoteMethodCall: entity(" + eid + ") not found!");
			return;
		}
		
		var methodUtype = 0;
		if(Ouroboros.moduledefs[entity.className].useMethodDescrAlias)
			methodUtype = stream.readUint8();
		else
			methodUtype = stream.readUint16();
		
		var methoddata = Ouroboros.moduledefs[entity.className].methods[methodUtype];
		var args = [];
		var argsdata = methoddata[3];
		for(var i=0; i<argsdata.length; i++)
		{
			args.push(argsdata[i].createFromStream(stream));
		}
		
		if(entity[methoddata[2]] != undefined)
		{
			entity[methoddata[2]].apply(entity, args);
		}
		else
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onRemoteMethodCall: entity(" + eid + ") not found method(" + methoddata[2] + ")!");
		}
	}
	
	this.Client_onRemoteMethodCallOptimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		Ouroboros.app.onRemoteMethodCall_(eid, stream);
	}
	
	this.Client_onRemoteMethodCall = function(stream)
	{
		var eid = stream.readInt32();
		Ouroboros.app.onRemoteMethodCall_(eid, stream);
	}
	
	this.Client_onEntityEnterWorld = function(stream)
	{
		var eid = stream.readInt32();
		if(Ouroboros.app.entity_id > 0 && eid != Ouroboros.app.entity_id)
			Ouroboros.app.entityIDAliasIDList.push(eid)
		
		var entityType;
		if(Ouroboros.moduledefs.Length > 255)
			entityType = stream.readUint16();
		else
			entityType = stream.readUint8();
		
		var isOnGround = true;
		
		if(stream.length() > 0)
			isOnGround = stream.readInt8();
		
		entityType = Ouroboros.moduledefs[entityType].name;
		Ouroboros.INFO_MSG("OuroborosApp::Client_onEntityEnterWorld: " + entityType + "(" + eid + "), spaceID(" + Ouroboros.app.spaceID + "), isOnGround(" + isOnGround + ")!");
		
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			var entityMessage = Ouroboros.bufferedCreateEntityMessages[eid];
			if(entityMessage == undefined)
			{
				Ouroboros.ERROR_MSG("OuroborosApp::Client_onEntityEnterWorld: entity(" + eid + ") not found!");
				return;
			}
			
			var runclass = Ouroboros.app.getentityclass(entityType);
			if(runclass == undefined)
				return;
			
			var entity = new runclass();
			entity.id = eid;
			entity.className = entityType;
			
			entity.cell = new Ouroboros.EntityCall();
			entity.cell.id = eid;
			entity.cell.className = entityType;
			entity.cell.type = Ouroboros.ENTITYCALL_TYPE_CELL;
			
			Ouroboros.app.entities[eid] = entity;
			
			Ouroboros.app.Client_onUpdatePropertys(entityMessage);
			delete Ouroboros.bufferedCreateEntityMessages[eid];
			
			entity.isOnGround = isOnGround > 0;
			entity.__init__();
			entity.inited = true;
			entity.inWorld = true;
			entity.enterWorld();
			
			if(Ouroboros.app.args.isOnInitCallPropertysSetMethods)
				entity.callPropertysSetMethods();
			
			entity.set_direction(entity.direction);
			entity.set_position(entity.position);
		}
		else
		{
			if(!entity.inWorld)
			{
				entity.cell = new Ouroboros.EntityCall();
				entity.cell.id = eid;
				entity.cell.className = entityType;
				entity.cell.type = Ouroboros.ENTITYCALL_TYPE_CELL;

				// 安全起�?， 这里清空一下
				// 如果�?务端上使用giveClientTo切�?�控制�?�
				// 之�?的实体已�?进入世界， 切�?��?�的实体也进入世界， 这里�?�能会残留之�?那个实体进入世界的信�?�
				Ouroboros.app.entityIDAliasIDList = [];
				Ouroboros.app.entities = {}
				Ouroboros.app.entities[entity.id] = entity;

				entity.set_direction(entity.direction);
				entity.set_position(entity.position);

				Ouroboros.app.entityServerPos.x = entity.position.x;
				Ouroboros.app.entityServerPos.y = entity.position.y;
				Ouroboros.app.entityServerPos.z = entity.position.z;
				
				entity.isOnGround = isOnGround > 0;
				entity.inWorld = true;
				entity.enterWorld();
				
				if(Ouroboros.app.args.isOnInitCallPropertysSetMethods)
					entity.callPropertysSetMethods();
			}
		}
	}

	this.Client_onEntityLeaveWorldOptimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		Ouroboros.app.Client_onEntityLeaveWorld(eid);
	}
	
	this.Client_onEntityLeaveWorld = function(eid)
	{
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onEntityLeaveWorld: entity(" + eid + ") not found!");
			return;
		}
		
		if(entity.inWorld)
			entity.leaveWorld();
		
		if(Ouroboros.app.entity_id > 0 && eid != Ouroboros.app.entity_id)
		{
			var newArray0 = [];

			for(var i=0; i<Ouroboros.app.controlledEntities.length; i++)
			{
				if(Ouroboros.app.controlledEntities[i] != eid)
				{
			       	newArray0.push(Ouroboros.app.controlledEntities[i]);
				}
				else
				{
					Ouroboros.Event.fire(Ouroboros.EventTypes.onLoseControlledEntity);
				}
			}
			
			Ouroboros.app.controlledEntities = newArray0
				
			delete Ouroboros.app.entities[eid];
			
			var newArray = [];
			for(var i=0; i<Ouroboros.app.entityIDAliasIDList.length; i++)
			{
				if(Ouroboros.app.entityIDAliasIDList[i] != eid)
				{
					newArray.push(Ouroboros.app.entityIDAliasIDList[i]);
				}
			}
			
			Ouroboros.app.entityIDAliasIDList = newArray
		}
		else
		{
			Ouroboros.app.clearSpace(false);
			entity.cell = null;
		}
	}

	this.Client_onEntityDestroyed = function(eid)
	{
		Ouroboros.INFO_MSG("OuroborosApp::Client_onEntityDestroyed: entity(" + eid + ")!");
		
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onEntityDestroyed: entity(" + eid + ") not found!");
			return;
		}

		if(entity.inWorld)
		{
			if(Ouroboros.app.entity_id == eid)
				Ouroboros.app.clearSpace(false);

			entity.leaveWorld();
		}
			
		delete Ouroboros.app.entities[eid];
	}
	
	this.Client_onEntityEnterSpace = function(stream)
	{
		var eid = stream.readInt32();
		Ouroboros.app.spaceID = stream.readUint32();
		var isOnGround = true;
		
		if(stream.length() > 0)
			isOnGround = stream.readInt8();
		
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onEntityEnterSpace: entity(" + eid + ") not found!");
			return;
		}
		
		entity.isOnGround = isOnGround;
		Ouroboros.app.entityServerPos.x = entity.position.x;
		Ouroboros.app.entityServerPos.y = entity.position.y;
		Ouroboros.app.entityServerPos.z = entity.position.z;
		entity.enterSpace();
	}
	
	this.Client_onEntityLeaveSpace = function(eid)
	{
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onEntityLeaveSpace: entity(" + eid + ") not found!");
			return;
		}
		
		Ouroboros.app.clearSpace(false);
		entity.leaveSpace();
	}

	this.Client_onKicked = function(failedcode)
	{
		Ouroboros.ERROR_MSG("OuroborosApp::Client_onKicked: failedcode=" + failedcode + "(" + Ouroboros.app.serverErrs[failedcode].name + ")!");
		Ouroboros.Event.fire(Ouroboros.EventTypes.onKicked, failedcode);
	}

	this.Client_onCreateAccountResult = function(stream)
	{
		var retcode = stream.readUint16();
		var datas = stream.readBlob();
		
		Ouroboros.Event.fire(Ouroboros.EventTypes.onCreateAccountResult, retcode, datas);
		
		if(retcode != 0)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onCreateAccountResult: " + Ouroboros.app.username + " create is failed! code=" + retcode + "(" + Ouroboros.app.serverErrs[retcode].name + "!");
			return;
		}

		Ouroboros.INFO_MSG("OuroborosApp::Client_onCreateAccountResult: " + Ouroboros.app.username + " create is successfully!");
	}

	this.Client_onControlEntity = function(eid, isControlled)
	{
		var eid = stream.readInt32();
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onControlEntity: entity(" + eid + ") not found!");
			return;
		}
		
		var isCont = isControlled != 0;
		if (isCont)
		{
			// 如果被控制者是玩家自己，那表示玩家自己被其它人控制了
			// 所以玩家自己�?应该进入这个被控制列表
			if (Ouroboros.app.player().id != entity.id)
			{
				Ouroboros.app.controlledEntities.push(entity)
			}
		}
		else
		{
			var newArray = [];

			for(var i=0; i<Ouroboros.app.controlledEntities.length; i++)
				if(Ouroboros.app.controlledEntities[i] != entity.id)
			       	newArray.push(Ouroboros.app.controlledEntities[i]);
			
			Ouroboros.app.controlledEntities = newArray
		}
		
		entity.isControlled = isCont;
		
		try
		{
			entity.onControlled(isCont);
			Ouroboros.Event.fire(Ouroboros.EventTypes.onControlled, entity, isCont);
		}
		catch (e)
		{
			Ouroboros.ERROR_MSG("Ouroboros::Client_onControlEntity: entity id = '" + eid + "', is controlled = '" + isCont + "', error = '" + e + "'");
		}
	}

	this.updatePlayerToServer = function()
	{
		var player = Ouroboros.app.player();
		if(player == undefined || player.inWorld == false || Ouroboros.app.spaceID == 0 || player.isControlled)
			return;
		
		if(player.entityLastLocalPos.distance(player.position) > 0.001 || player.entityLastLocalDir.distance(player.direction) > 0.001)
		{
			// 记录玩家最�?�一次上报�?置时自身当�?的�?置
			player.entityLastLocalPos.x = player.position.x;
			player.entityLastLocalPos.y = player.position.y;
			player.entityLastLocalPos.z = player.position.z;
			player.entityLastLocalDir.x = player.direction.x;
			player.entityLastLocalDir.y = player.direction.y;
			player.entityLastLocalDir.z = player.direction.z;	
							
			var bundle = Ouroboros.Bundle.createObject();
			bundle.newMessage(Ouroboros.messages.Baseapp_onUpdateDataFromClient);
			bundle.writeFloat(player.position.x);
			bundle.writeFloat(player.position.y);
			bundle.writeFloat(player.position.z);
			bundle.writeFloat(player.direction.x);
			bundle.writeFloat(player.direction.y);
			bundle.writeFloat(player.direction.z);
			bundle.writeUint8(player.isOnGround);
			bundle.writeUint32(Ouroboros.app.spaceID);
			bundle.send(Ouroboros.app);
		}
		
		// 开始�?�步所有被控制了的entity的�?置
		for (var eid in Ouroboros.app.controlledEntities)  
		{ 
			var entity = Ouroboros.app.controlledEntities[i];
			position = entity.position;
			direction = entity.direction;

			posHasChanged = entity.entityLastLocalPos.distance(position) > 0.001;
			dirHasChanged = entity.entityLastLocalDir.distance(direction) > 0.001;

			if (posHasChanged || dirHasChanged)
			{
				entity.entityLastLocalPos = position;
				entity.entityLastLocalDir = direction;

				var bundle = Ouroboros.Bundle.createObject();
				bundle.newMessage(Ouroboros.messages.Baseapp_onUpdateDataFromClientForControlledEntity);
				bundle.writeInt32(entity.id);
				bundle.writeFloat(position.x);
				bundle.writeFloat(position.y);
				bundle.writeFloat(position.z);

				bundle.writeFloat(direction.x);
				bundle.writeFloat(direction.y);
				bundle.writeFloat(direction.z);
				bundle.writeUint8(entity.isOnGround);
				bundle.writeUint32(Ouroboros.app.spaceID);
				bundle.send(Ouroboros.app);
			}
		}
	}
	
	this.addSpaceGeometryMapping = function(spaceID, respath)
	{
		Ouroboros.INFO_MSG("OuroborosApp::addSpaceGeometryMapping: spaceID(" + spaceID + "), respath(" + respath + ")!");
		
		Ouroboros.app.spaceID = spaceID;
		Ouroboros.app.spaceResPath = respath;
		Ouroboros.Event.fire(Ouroboros.EventTypes.addSpaceGeometryMapping, respath);
	}

	this.clearSpace = function(isAll)
	{
		Ouroboros.app.entityIDAliasIDList = [];
		Ouroboros.app.spacedata = {};
		Ouroboros.app.clearEntities(isAll);
		Ouroboros.app.isLoadedGeometry = false;
		Ouroboros.app.spaceID = 0;
	}

	this.clearEntities = function(isAll)
	{
		Ouroboros.app.controlledEntities = []

		if(!isAll)
		{
			var entity = Ouroboros.app.player();
			
			for (var eid in Ouroboros.app.entities)  
			{ 
				if(eid == entity.id)
					continue;
				
				if(Ouroboros.app.entities[eid].inWorld)
				{
			    	Ouroboros.app.entities[eid].leaveWorld();
			    }
			    
			    Ouroboros.app.entities[eid].onDestroy();
			}  
				
			Ouroboros.app.entities = {}
			Ouroboros.app.entities[entity.id] = entity;
		}
		else
		{
			for (var eid in Ouroboros.app.entities)  
			{ 
				if(Ouroboros.app.entities[eid].inWorld)
			    {
			    	Ouroboros.app.entities[eid].leaveWorld();
			    }
			    
			    Ouroboros.app.entities[eid].onDestroy();
			}  
				
			Ouroboros.app.entities = {}
		}
	}

	this.Client_initSpaceData = function(stream)
	{
		Ouroboros.app.clearSpace(false);
		
		Ouroboros.app.spaceID = stream.readInt32();
		while(stream.length() > 0)
		{
			var key = stream.readString();
			var value = stream.readString();
			Ouroboros.app.Client_setSpaceData(Ouroboros.app.spaceID, key, value);
		}
		
		Ouroboros.INFO_MSG("OuroborosApp::Client_initSpaceData: spaceID(" + Ouroboros.app.spaceID + "), datas(" + Ouroboros.app.spacedata + ")!");
	}
	
	this.Client_setSpaceData = function(spaceID, key, value)
	{
		Ouroboros.INFO_MSG("OuroborosApp::Client_setSpaceData: spaceID(" + spaceID + "), key(" + key + "), value(" + value + ")!");
		
		Ouroboros.app.spacedata[key] = value;
		
		if(key == "_mapping")
			Ouroboros.app.addSpaceGeometryMapping(spaceID, value);
		
		Ouroboros.Event.fire(Ouroboros.EventTypes.onSetSpaceData, spaceID, key, value);
	}
	
	this.Client_delSpaceData = function(spaceID, key)
	{
		Ouroboros.INFO_MSG("OuroborosApp::Client_delSpaceData: spaceID(" + spaceID + "), key(" + key + ")!");
		
		delete Ouroboros.app.spacedata[key];
		Ouroboros.Event.fire(Ouroboros.EventTypes.onDelSpaceData, spaceID, key);
	}
	
	this.Client_getSpaceData = function(spaceID, key)
	{
		return Ouroboros.app.spacedata[key];
	}
	
	this.Client_onUpdateBasePos = function(x, y, z)
	{
		Ouroboros.app.entityServerPos.x = x;
		Ouroboros.app.entityServerPos.y = y;
		Ouroboros.app.entityServerPos.z = z;
	}
	
	this.Client_onUpdateBasePosXZ = function(x, z)
	{
		Ouroboros.app.entityServerPos.x = x;
		Ouroboros.app.entityServerPos.z = z;
	}
	
	this.Client_onUpdateData = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onUpdateData: entity(" + eid + ") not found!");
			return;
		}
	}

	this.Client_onSetEntityPosAndDir = function(stream)
	{
		var eid = stream.readInt32();
		var entity = Ouroboros.app.entities[eid];
		if(entity == undefined)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onSetEntityPosAndDir: entity(" + eid + ") not found!");
			return;
		}
		
		entity.position.x = stream.readFloat();
		entity.position.y = stream.readFloat();
		entity.position.z = stream.readFloat();
		entity.direction.x = stream.readFloat();
		entity.direction.y = stream.readFloat();
		entity.direction.z = stream.readFloat();
		
		// 记录玩家最�?�一次上报�?置时自身当�?的�?置
		entity.entityLastLocalPos.x = entity.position.x;
		entity.entityLastLocalPos.y = entity.position.y;
		entity.entityLastLocalPos.z = entity.position.z;
		entity.entityLastLocalDir.x = entity.direction.x;
		entity.entityLastLocalDir.y = entity.direction.y;
		entity.entityLastLocalDir.z = entity.direction.z;	
				
		entity.set_direction(entity.direction);
		entity.set_position(entity.position);
	}
	
	this.Client_onUpdateData_ypr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readFloat();
		var p = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, p, r, -1, false);
	}
	
	this.Client_onUpdateData_yp = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readFloat();
		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, p, Ouroboros.OURO_FLT_MAX, -1, false);
	}
	
	this.Client_onUpdateData_yr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, Ouroboros.OURO_FLT_MAX, r, -1, false);
	}
	
	this.Client_onUpdateData_pr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var p = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, p, r, -1, false);
	}
	
	this.Client_onUpdateData_y = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, -1, false);
	}
	
	this.Client_onUpdateData_p = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, p, Ouroboros.OURO_FLT_MAX, -1, false);
	}
	
	this.Client_onUpdateData_r = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, r, -1, false);
	}
	
	this.Client_onUpdateData_xz = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 1, false);
	}
	
	this.Client_onUpdateData_xz_ypr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var y = stream.readFloat();
		var p = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, y, p, r, 1, false);
	}
	
	this.Client_onUpdateData_xz_yp = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var y = stream.readFloat();
		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, y, p, Ouroboros.OURO_FLT_MAX, 1, false);
	}
	
	this.Client_onUpdateData_xz_yr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var y = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, y, Ouroboros.OURO_FLT_MAX, r, 1, false);
	}
	
	this.Client_onUpdateData_xz_pr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var p = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, Ouroboros.OURO_FLT_MAX, p, r, 1, false);
	}
	
	this.Client_onUpdateData_xz_y = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var y = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, y, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 1, false);
	}
	
	this.Client_onUpdateData_xz_p = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], Ouroboros.OURO_FLT_MAX, p, Ouroboros.OURO_FLT_MAX, 1, false);
	}
	
	this.Client_onUpdateData_xz_r = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var z = stream.readFloat();

		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, Ouroboros.OURO_FLT_MAX, z, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, r, 1, false);
	}
	
	this.Client_onUpdateData_xyz = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 0, false);
	}
	
	this.Client_onUpdateData_xyz_ypr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var yaw = stream.readFloat();
		var p = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, yaw, p, r, 0, false);
	}
	
	this.Client_onUpdateData_xyz_yp = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var yaw = stream.readFloat();
		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, yaw, p, Ouroboros.OURO_FLT_MAX, 0, false);
	}
	
	this.Client_onUpdateData_xyz_yr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var yaw = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, yaw, Ouroboros.OURO_FLT_MAX, r, 0, false);
	}
	
	this.Client_onUpdateData_xyz_pr = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var p = stream.readFloat();
		var r = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, Ouroboros.OURO_FLT_MAX, p, r, 0, false);
	}
	
	this.Client_onUpdateData_xyz_y = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var yaw = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, yaw, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 0, false);
	}
	
	this.Client_onUpdateData_xyz_p = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, Ouroboros.OURO_FLT_MAX, p, Ouroboros.OURO_FLT_MAX, 0, false);
	}
	
	this.Client_onUpdateData_xyz_r = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var x = stream.readFloat();
		var y = stream.readFloat();
		var z = stream.readFloat();
		
		var p = stream.readFloat();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, r, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 0, false);
	}
	
	//--------------------optiom------------------------//
	this.Client_onUpdateData_ypr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readInt8();
		var p = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, p, r, -1, true);
	}
	
	this.Client_onUpdateData_yp_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readInt8();
		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, p, Ouroboros.OURO_FLT_MAX, -1, true);
	}
	
	this.Client_onUpdateData_yr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, Ouroboros.OURO_FLT_MAX, r, -1, true);
	}
	
	this.Client_onUpdateData_pr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var p = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, p, r, -1, true);
	}
	
	this.Client_onUpdateData_y_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var y = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, y, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, -1, true);
	}
	
	this.Client_onUpdateData_p_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, p, Ouroboros.OURO_FLT_MAX, -1, true);
	}
	
	this.Client_onUpdateData_r_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, r, -1, true);
	}
	
	this.Client_onUpdateData_xz_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 1, true);
	}
	
	this.Client_onUpdateData_xz_ypr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var y = stream.readInt8();
		var p = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], y, p, r, 1, true);
	}
	
	this.Client_onUpdateData_xz_yp_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var y = stream.readInt8();
		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], y, p, Ouroboros.OURO_FLT_MAX, 1, true);
	}
	
	this.Client_onUpdateData_xz_yr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var y = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], y, Ouroboros.OURO_FLT_MAX, r, 1, true);
	}
	
	this.Client_onUpdateData_xz_pr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var p = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], Ouroboros.OURO_FLT_MAX, p, r, 1, true);
	}
	
	this.Client_onUpdateData_xz_y_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var y = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], y, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 1, true);
	}
	
	this.Client_onUpdateData_xz_p_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], Ouroboros.OURO_FLT_MAX, p, Ouroboros.OURO_FLT_MAX, 1, true);
	}
	
	this.Client_onUpdateData_xz_r_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();

		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], Ouroboros.OURO_FLT_MAX, xz[1], Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, r, 1, true);
	}
	
	this.Client_onUpdateData_xyz_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 0, true);
	}
	
	this.Client_onUpdateData_xyz_ypr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var yaw = stream.readInt8();
		var p = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], yaw, p, r, 0, true);
	}
	
	this.Client_onUpdateData_xyz_yp_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var yaw = stream.readInt8();
		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], yaw, p, Ouroboros.OURO_FLT_MAX, 0, true);
	}
	
	this.Client_onUpdateData_xyz_yr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var yaw = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], yaw, Ouroboros.OURO_FLT_MAX, r, 0, true);
	}
	
	this.Client_onUpdateData_xyz_pr_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var p = stream.readInt8();
		var r = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, x, y, z, Ouroboros.OURO_FLT_MAX, p, r, 0, true);
	}
	
	this.Client_onUpdateData_xyz_y_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var yaw = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], yaw, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 0, true);
	}
	
	this.Client_onUpdateData_xyz_p_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], Ouroboros.OURO_FLT_MAX, p, Ouroboros.OURO_FLT_MAX, 0, true);
	}
	
	this.Client_onUpdateData_xyz_r_optimized = function(stream)
	{
		var eid = Ouroboros.app.getViewEntityIDFromStream(stream);
		
		var xz = stream.readPackXZ();
		var y = stream.readPackY();
		
		var p = stream.readInt8();
		
		Ouroboros.app._updateVolatileData(eid, xz[0], y, xz[1], r, Ouroboros.OURO_FLT_MAX, Ouroboros.OURO_FLT_MAX, 0, true);
	}

	this._updateVolatileData = function(entityID, x, y, z, yaw, pitch, roll, isOnGround, isOptimized)
	{
		var entity = Ouroboros.app.entities[entityID];
		if(entity == undefined)
		{
			// 如果为0且客户端上一步是�?登陆或者�?连�?作并且�?务端entity在断线期间一直处于在线状�?
			// 则�?�以忽略这个错误, 因为cellapp�?�能一直在�?�baseapp�?��?�?�步消�?�， 当客户端�?连上时未等
			// �?务端�?始化步骤开始则收到�?�步信�?�, 此时这里就会出错。			
			Ouroboros.ERROR_MSG("OuroborosApp::_updateVolatileData: entity(" + entityID + ") not found!");
			return;
		}
		
		// �?于0�?设置
		if(isOnGround >= 0)
		{
			entity.isOnGround = (isOnGround > 0);
		}
		
		var changeDirection = false;
		
		if(roll != Ouroboros.OURO_FLT_MAX)
		{
			changeDirection = true;
			entity.direction.x = isOptimized ? Ouroboros.int82angle(roll, false) : roll;
		}

		if(pitch != Ouroboros.OURO_FLT_MAX)
		{
			changeDirection = true;
			entity.direction.y = isOptimized ? Ouroboros.int82angle(pitch, false) : pitch;
		}
		
		if(yaw != Ouroboros.OURO_FLT_MAX)
		{
			changeDirection = true;
			entity.direction.z = isOptimized ? Ouroboros.int82angle(yaw, false) : yaw;
		}
		
		var done = false;
		if(changeDirection == true)
		{
			Ouroboros.Event.fire(Ouroboros.EventTypes.set_direction, entity);		
			done = true;
		}
		
		var positionChanged = false;
		if(x != Ouroboros.OURO_FLT_MAX || y != Ouroboros.OURO_FLT_MAX || z != Ouroboros.OURO_FLT_MAX)
			positionChanged = true;

		if (x == Ouroboros.OURO_FLT_MAX) x = isOptimized ? 0.0 : entity.position.x;
		if (y == Ouroboros.OURO_FLT_MAX) y = isOptimized ? 0.0 : entity.position.y;
		if (z == Ouroboros.OURO_FLT_MAX) z = isOptimized ? 0.0 : entity.position.z;
        
		if(positionChanged)
		{
			if(isOptimized)
			{
				entity.position.x = x + Ouroboros.app.entityServerPos.x;
				entity.position.y = y + Ouroboros.app.entityServerPos.y;
				entity.position.z = z + Ouroboros.app.entityServerPos.z;
			}
			else
			{
				entity.position.x = x;
				entity.position.y = y;
				entity.position.z = z;
			}
			
			done = true;
			Ouroboros.Event.fire(Ouroboros.EventTypes.updatePosition, entity);
		}
		
		if(done)
			entity.onUpdateVolatileData();		
	}
	
	this.Client_onStreamDataStarted = function(id, datasize, descr)
	{
		Ouroboros.Event.fire(Ouroboros.EventTypes.onStreamDataStarted, id, datasize, descr);
	}
	
	this.Client_onStreamDataRecv = function(stream)
	{
		var id = stream.readUint16();
		var data = stream.readBlob();
		Ouroboros.Event.fire(Ouroboros.EventTypes.onStreamDataRecv, id, data);
	}
	
	this.Client_onStreamDataCompleted = function(id)
	{
		Ouroboros.Event.fire(Ouroboros.EventTypes.onStreamDataCompleted, id);
	}
	
	this.Client_onReqAccountResetPasswordCB = function(failedcode)
	{
		if(failedcode != 0)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onReqAccountResetPasswordCB: " + Ouroboros.app.username + " is failed! code=" + failedcode + "(" + Ouroboros.app.serverErrs[failedcode].name + ")!");
			return;
		}

		Ouroboros.INFO_MSG("OuroborosApp::Client_onReqAccountResetPasswordCB: " + Ouroboros.app.username + " is successfully!");
	}
	
	this.Client_onReqAccountBindEmailCB = function(failedcode)
	{
		if(failedcode != 0)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onReqAccountBindEmailCB: " + Ouroboros.app.username + " is failed! code=" + failedcode +"(" + Ouroboros.app.serverErrs[failedcode].name + ")!");
			return;
		}

		Ouroboros.INFO_MSG("OuroborosApp::Client_onReqAccountBindEmailCB: " + Ouroboros.app.username + " is successfully!");
	}
	
	this.Client_onReqAccountNewPasswordCB = function(failedcode)
	{
		if(failedcode != 0)
		{
			Ouroboros.ERROR_MSG("OuroborosApp::Client_onReqAccountNewPasswordCB: " + Ouroboros.app.username + " is failed! code=" + failedcode + "(" +Ouroboros.app.serverErrs[failedcode].name + ")!");
			return;
		}

		Ouroboros.INFO_MSG("OuroborosApp::Client_onReqAccountNewPasswordCB: " + Ouroboros.app.username + " is successfully!");
	}
}

Ouroboros.create = function(ouroborosArgs)
{
	if(Ouroboros.app != undefined)
		return;

	// 一些平�?�如�?程�?上�?�能没有assert
	if(console.assert == undefined)
	{
		console.assert = function(bRet, s)
		{
			if(!(bRet)) {
				Ouroboros.ERROR_MSG(s);
			}
		}
	}

	if(ouroborosArgs.constructor != Ouroboros.OuroborosArgs)
	{
		Ouroboros.ERROR_MSG("Ouroboros.create(): args(" + ouroborosArgs + ") error! not is Ouroboros.OuroborosArgs");
		return;
	}
	
	new Ouroboros.OuroborosApp(ouroborosArgs);
	
	Ouroboros.app.reset();
	Ouroboros.app.installEvents();
	Ouroboros.idInterval = setInterval(Ouroboros.app.update, ouroborosArgs.updateHZ);
}

Ouroboros.destroy = function()
{
	if(Ouroboros.idInterval != undefined)
		clearInterval(Ouroboros.idInterval);
	
	if(Ouroboros.app == undefined)
		return;
		
	Ouroboros.app.uninstallEvents();
	Ouroboros.app.reset();
	Ouroboros.app = undefined;

	Ouroboros.Event.clear();
}

try
{
	if(module != undefined)
	{
		module.exports = Ouroboros;
	}
}
catch(e)
{
	
}


