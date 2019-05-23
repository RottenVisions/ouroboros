/*
 * Form validation plug-in easyform
 * Author:LeeLanfei
 * 2014-11-5
 * For form validation
 * As long as the need to validate the controls on the Add easyform properties, multiple properties with the[
 * Attribute list:
 *      null
 *      email
 *      char-normal         English, numbers, underscores
 *      char-chinese        Chinese, English, numbers, underscores, Chinese punctuation
 *      char-english        English, numbers, underscores, English punctuation
 *      length:1-10 / length:4
 *      equal:xxx                               Equal to the value of an object, and after the colon is the jq selector syntax
 *      ajax:fun()
 *      real-time                               Real-time check
 *      date                    2014-10-31
 *      time                    10:30:00
 *      datetime            2014-10-31 10:30:00
 *      money               A positive number, two decimal places
 *      uint :1                 A positive integer , parameters as a starting value
 *
 *
 *  ------ requirement list ----------------------------------------------------
 * 1. 2014-11-18 Does not exclude the hidden input and the hidden type of input
 * 2. 2014-11-18 The need to support a conditional message.
 * 3. 2014-11-19 ajax does not support asynchronous
 * 4. 2014-11-19 Not considering the file type and other special type of judgment
 * 5. 2014-11-20 When the page load when there are hidden controls, after the control is displayed, its associated easytip does not correctly display the location
 * 6. 2014-11-21 Does not currently support attribute inheritance
 * 7. 2014-11-21 Real-time check the time, the pop-up easytip sometimes cause the pop-up information message Error
 *
 *
 * ------ change list -------------------------------------------------
 * 1. 2014-11-18 requirement list 1 Complete
 * 2. 2014-11-18 Support real-time check
 * 3. 2014-11-18 requirement list 2 Complete
 * 4. 2014-11-20 requirement list 3 Support ajax-asynchronous authentication mode.
 * 5. 2014-11-21 requirement list 5 complete
 *
 * */
;
(function ($, window, document, undefined)
{
    /*
     Constructor
     **/
    var _easyform = function (ele, opt)
    {
        this.form = ele;

        this.defaults = {
            easytip: true
        };
        this.options = $.extend({}, this.defaults, opt);

        this.result = [];
        this.inputs = [];

        this.counter = 0;   //It has been determined that the success of the input count
        this.is_submit = true;


        //Event definition
        this.success = null;
        this.error = null;
    };

    //Method
    _easyform.prototype = {

        init: function ()
        {
            var ei = this;
            ei._load();

            //Overwrite the submit attribute, easy to control
            this.submit_button = this.form.find("input:submit");
            this.submit_button.each(function ()
            {
                var button = $(this);
                button.attr("type", "button");

                //Submitted before the judge
                button.click(function ()
                {
                    ei.submit(true);

                });
            });

            return this;
        },

        _load: function ()
        {
            this.inputs.splice(0, this.inputs.length);
            var ev = this;

            this.form.find("input:visible").each(function (index, input)
            {
                //排除 hidden�?button�?submit�?checkbox�?radio�?file
                if (input.type != "hidden" && input.type != "button" && input.type != "submit" && input.type != "checkbox" && input.type != "radio" && input.type != "file")
                {
                    var checker = $(input).easyinput({easytip: ev.easytip});

                    checker.error = function (e)
                    {
                        ev.is_submit = false;
                        ev.result.push(e);

                        if (!!ev.error)    //Failed event
                            ev.error(e);
                    };

                    checker.success = function (e)
                    {
                        ev.counter++;
                        if (ev.counter == ev.inputs.length)
                        {
                            ev.counter = 0;

                            if (!!ev.success)    //The success of the event
                                ev.success();

                            if (!!ev.is_submit)
                            {
                                ev.form.submit();
                            }
                        }
                    };

                    ev.inputs.push(checker);
                }
            });
        },

        /*
         * Form submit function
         * @submit: a bool value that defines whether really to submit the form
         * */
        submit: function (submit)
        {

            this._load();                                               //Re-load controls
            this.result.splice(0, this.result.length);     //Empty the previous results

            this.counter = 0;
            this.is_submit = submit;


            var index;
            for (index in this.inputs)
            {
                this.inputs[index].validation();
            }
        }

    };

    //Add to jquery
    $.fn.easyform = function (options)
    {
        var validation = new _easyform(this, options);

        return validation.init();
    };


})(jQuery, window, document);

(function ($, window, document, undefined)
{
    //A single input to the checker constructor
    var _easyinput = function (input, opt)
    {
        this.input = input;
        this.rules = [];

        this.message = $(input).attr("message");
        this.message = (!!this.message ? this.message : "格�?错误!");

        //Event
        this.error = null;
        this.success = null;

        this.defaults = {
            easytip: true   //Whether to display the easytip
        };
        this.options = $.extend({}, this.defaults, opt);

        this.counter = 0;   //Counter, recording have how many conditions successfully

        this.is_error = false;
    };

    //A single input to the checker
    _easyinput.prototype = {

        init: function ()
        {
            //Initialize the easytip
            this._init_easytip();

            //Whether the real-time check
            var easyinput = this;
            var rule = this.input.attr("easyform");
            if (!!rule && -1 != rule.indexOf("real-time"))
            {
                this.input.blur(function ()
                {
                    easyinput.validation();
                });
            }

            return this;
        },

        //Initialize the easytip
        _init_easytip: function ()
        {
            if (!!this.options.easytip)
            {
                var tipoptions = $(this.input).attr("easytip");

                tipoptions = (!!tipoptions ? tipoptions.split(";") : undefined);

                if (!!tipoptions)
                {
                    var options = Object();
                    var index;
                    for (index in tipoptions)
                    {
                        var temps = tipoptions[index];
                        var p = temps.indexOf(":");

                        if (-1 == p)continue;

                        var temp = [];
                        temp[0] = temps.substring(0, p);
                        temp[1] = temps.substring(p + 1);

                        options[temp[0]] = temp[1];
                    }
                }

                this.options.easytip = $(this.input).easytip(options);
            }
        },

        /**
         * Judge Rules
         * */
        validation: function ()
        {
            this.value = this.input.val();
            this.counter = 0;   //The counter is cleared
            this.rule = this.input.attr("easyform");

            this.is_error = false;

            this._parse(this.rule);

            this._null(this, this.value, this.rule);

            for (var i = 0; i < this.rules.length; i++)
            {
                //Call the condition function
                if (!!this.judge[this.rules[i].rule])
                    this.judge[this.rules[i].rule](this, this.value, this.rules[i].param);
            }
        },

        //easyform Analytic functions
        _parse: function (str)
        {
            this.rules = [];

            var strs = !!str ? str.split(";") : {};

            for (var i = 0; i < strs.length; i++)
            {
                var s = strs[i];
                var rule = s;
                var param = "";

                //: No.
                var p = s.indexOf(":");
                if (-1 != p)
                {
                    rule = s.substr(0, p);
                    param = s.substr(p + 1);
                }

                if (!!this.judge[rule])
                    this.rules.push({rule: rule, param: param});
            }
        },

        _error: function (rule)
        {
            if (!!this.error)
                this.error(this.input, rule);

            if (false == this.is_error)
            {
                var msg = this.input.attr(rule + "-message");

                var msg = !msg ? this.message : msg;

                if (!!this.options.easytip)
                {
                    this.options.easytip.show(msg);
                }

                this.is_error = true;
            }

            return false;
        },

        _success: function ()
        {
            if (!!this.success)
                this.success(this.input);

            return true;
        },

        _success_rule: function (rule)
        {
            this.counter += 1;

            if (this.counter == this.rules.length)
                this._success();

            return true;
        },

        _null: function (ei, v, r)
        {
            if (!v)
            {
                if (!!r && -1 != r.indexOf("null")) //the rule is not empty and contains null
                {
                    return ei._success();
                }
                else
                {
                    return ei._error("require");
                }
            }
        },

        /*
         * According to various rule to judge of the function array
         * By the judge to add the member function, you can expand the rules
         * */
        judge: {
            "char-normal": function (ei, v, p)
            {
                if (false == /^\w+$/.test(v))
                    return ei._error("char-normal");
                else
                    return ei._success_rule("char-normal");
            },

            "char-chinese": function (ei, v, p)
            {
                if (false == /^([\w]|[\u4e00-\u9fa5]|[ 。，�?？￥“‘�?：�?】《》（）——+-])+$/.test(v))
                    return ei._error("char-chinese");
                else
                    return ei._success_rule("char-chinese");
            },

            "char-english": function (ei, v, p)
            {
                if (false == /^([\w]|[ .,?!$'":+-])+$/.test(v))
                    return ei._error("char-english");
                else
                    return ei._success_rule("char-english");
            },

            "email": function (ei, v, p)
            {
                if (false == /^[\w-]+(\.[\w-]+)*@[\w-]+(\.[\w-]+)+$/.test(v))
                    return ei._error("email");
                else
                    return ei._success_rule("email");
            },

            "length": function (ei, v, p)
            {
                var range = p.split("-");

                //If the length is set to a length:6 such a format
                if (range.length == 1) range[1] = range[0];

                if (v.length < range[0] || v.length > range[1])
                    return ei._error("length");
                else
                    return ei._success_rule("length");
            },

            "equal": function (ei, v, p)
            {
                var pair = $(p);
                if (0 == pair.length || pair.val() != v)
                    return ei._error("equal");
                else
                    return ei._success_rule("equal");
            },

            "ajax": function (ei, v, p)
            {
                // For ajax processing to register a custom event
                // HTML in the implementation of the relevant AJAX, you need to send the event easyinput-ajax to notify the easyinput
                // This event is only a bool parameter, easyinput will be based on this value determine the ajax validation is successful
                ei.input.delegate("", "easyinput-ajax", function (e, p)
                {
                    ei.input.unbind("easyinput-ajax");

                    if (false == p)
                        return ei._error("ajax");
                    else
                        return ei._success_rule("ajax");
                });

                eval(p);
            },

            "date": function (ei, v, p)
            {
                if (false == /^(\d{4})-(\d{2})-(\d{2})$/.test(v))
                    return ei._error("date");
                else
                    return ei._success_rule("date");
            },

            "time": function (ei, v, p)
            {
                if (false == /^(\d{2}):(\d{2}):(\d{2})$/.test(v))
                    return ei._error("time");
                else
                    return ei._success_rule(v);
            },

            "datetime": function (ei, v, p)
            {
                if (false == /^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})$/.test(v))
                    return ei._error("datetime");
                else
                    return ei._success_rule("datetime");
            },

            "money": function (ei, v, p)
            {
                if (false == /^([1-9][\d]{0,7}|0)(\.[\d]{1,2})?$/.test(v))
                    return ei._error("money");
                else
                    return ei._success_rule("money");
            },

            "uint": function (ei, v, p)
            {
                v = parseInt(v);
                p = parseInt(p);

                if (isNaN(v) || isNaN(p) || v < p || v < 0)
                    return ei._error("uint");
                else
                    return ei._success_rule("uint");
            }
        }
    };

    $.fn.easyinput = function (options)
    {
        var check = new _easyinput(this, options);


        return check.init();
    };

})(jQuery, window, document);

(function ($, window, document, undefined)
{
    var themes = {
        black: {
            color: "rgba(238,238,238,1)",
            "background-color": "rgba(75,75,75,0.8",
            "border": "1px solid rgba(75,75,75,1)",
            "border-radius": 5
        },
        blue: {
            color: "rgba(255,255,255,1)",
            "background-color": "rgba(51,153,204,0.8)",
            "border": "1px solid rgba(102,153,204,1)",
            "border-radius": 5
        },
        red: {
            color: "rgba(190,0,0,1)",
            "background-color": "rgba(255,255,255,0.9)",
            "border": "1px solid rgba(190,0,0,1)",
            "border-radius": 5
        },
        white: {
            color: "rgba(102,102,102,1)",
            "background-color": "rgba(255,255,255,0.9)",
            "border": "1px solid rgba(153,153,153,1)",
            "border-radius": 5
        }
    };

    var _easytip = function (ele, opt)
    {
        this.parent = ele;
        this.defaults = {
            left: 0, top: 0,
            position: "right",
            disappear: "other",        //self, other, lost-focus, none, N seconds
            speed: "fast",
            theme: "white",
            arrow: "bottom",        //top, left, bottom, right
            onshow: null,
            onclose: null,
            style: {}
        };
        this.options = $.extend({}, this.defaults, opt);
        this.theme = themes[this.options.theme];

        this.padding = 0;

        this.id = "easytip-div-main" + ele[0].id;
    };

    _easytip.prototype = {

        init: function ()
        {
            var tip = $("#" + this.id);

            if (tip.length == 0)
            {
                $(document.body).append("<div id=\"" + this.id + "\"><div class=\"easytip-text\"></div></div>");

                tip = $("#" + this.id);
                var text = $("#" + this.id + " .easytip-text");

                tip.css({
                    "margin-left":"15px",
                    "text-align": "left",
                    "display": "none",
                    "position": "absolute"
                });

                text.css({
                    "text-align": "left",
                    "padding": "5px",
                    "min-width": "120px"
                });

                // tip.append("<div class=\"easytip-arrow\"></div>");
                // var arrow = $("#" + this.id + " .easytip-arrow");
                // arrow.css({
                //     "padding": "0",
                //     "margin": "0",
                //     "width": "0",
                //     "height": "0",
                //     "position": "absolute",
                //     "border": "10px solid"
                // });
            }

            return this;
        },

        _size: function ()
        {
            var parent = this.parent;
            var tip = $("#" + this.id);


            if (tip.width() > 300)
            {
                tip.width(300);
            }
        },

        _css: function ()
        {
            var tip = $("#" + this.id);
            var text = $("#" + this.id + " .easytip-text");
            var arrow = $("#" + this.id + " .easytip-arrow");

            text.css(this.theme);

            arrow.css("border-color", "transparent transparent transparent transparent");

            if (this.options.style != null && typeof(this.options.style) == "object")
            {
                text.css(this.options.style);
            }
        },

        _arrow: function ()
        {
            var tip = $("#" + this.id);
            var text = $("#" + this.id + " .easytip-text");
            var arrow = $("#" + this.id + " .easytip-arrow");

            switch (this.options.arrow)
            {
                case "top":
                    arrow.css({
                        "left": "25px",
                        "top": -arrow.outerHeight(),
                        "border-bottom-color": text.css("borderTopColor")
                    });
                    break;

                case "left":
                    arrow.css({
                        "left": -arrow.outerWidth(),
                        "top": tip.innerHeight() / 2 - arrow.outerHeight() / 2,
                        "border-right-color": text.css("borderTopColor")
                    });
                    break;

                case "bottom":
                    arrow.css({
                        "left": "25px",
                        "top": tip.innerHeight(),
                        "border-top-color": text.css("borderTopColor")
                    });
                    break;

                case "right":
                    arrow.css({
                        "left": tip.outerWidth(),
                        "top": tip.innerHeight() / 2 - arrow.outerHeight() / 2,
                        "border-left-color": text.css("borderTopColor")
                    });
                    break;
            }
        },

        _position: function ()
        {
            var tip = $("#" + this.id);
            var text = $("#" + this.id + " .easytip-text");
            var arrow = $("#" + this.id + " .easytip-arrow");
            var offset = $(this.parent).offset();
            var size = {width: $(this.parent).outerWidth(), height: $(this.parent).outerHeight()};

            switch (this.options.position)
            {
                case "top":

                    tip.css("left", offset.left - this.padding);
                    tip.css("top", offset.top - tip.outerHeight() - arrow.outerHeight() / 2);
                    this.options.arrow = "bottom";

                    break;

                case "left":

                    tip.css("left", offset.left - tip.outerWidth() - arrow.outerWidth() / 2);
                    tip.css("top", offset.top - (tip.outerHeight() - size.height) / 2);
                    this.options.arrow = "right";

                    break;

                case "bottom":

                    tip.css("left", offset.left - this.padding);
                    tip.css("top", offset.top + size.height + arrow.outerHeight() / 2);
                    this.options.arrow = "top";

                    break;

                case "right":

                    tip.css("left", offset.left + size.width + arrow.outerWidth() / 2);
                    tip.css("top", offset.top - (tip.outerHeight() - size.height) / 2);
                    this.options.arrow = "left";

                    break;
            }

            var left = parseInt(tip.css("left"));
            var top = parseInt(tip.css("top"));

            tip.css("left", parseInt(this.options.left) + left);
            tip.css("top", parseInt(this.options.top) + top);
        },

        show: function (msg)
        {
            var tip = $("#" + this.id);
            var text = $("#" + this.id + " .easytip-text");
            var arrow = $("#" + this.id + " .easytip-arrow");
            var speed = this.options.speed;
            var disappear = this.options.disappear;
            var parent = this.parent;

            text.html(msg);

            this._size();
            this._css();
            this._position();
            this._arrow();

            var onshow = this.options.onshow;
            var onclose = this.options.onclose;

            tip.fadeIn(speed, function ()
            {
                if (!!onshow)    onshow({parent: parent, target: tip[0]});

                if (!isNaN(disappear))
                {
                    setTimeout(function ()
                    {

                        tip.fadeOut(speed, function ()
                        {
                            if (!!onclose)    onclose({parent: parent, target: tip[0]});
                        });

                    }, disappear);
                }
                else if (disappear == "self" || disappear == "other")
                {
                    $(document).click(function (e)
                    {
                        if (disappear == "self" && e.target == text[0])
                        {
                            tip.fadeOut(speed, function ()
                            {
                                if (!!onclose)    onclose({parent: parent, target: tip[0]});
                                $(document).unbind("click");
                            });
                        }
                        else if (disappear == "other" && e.target != tip[0])
                        {
                            tip.fadeOut(speed, function ()
                            {
                                if (!!onclose)    onclose({parent: parent, target: tip[0]});
                                $(document).unbind("click");
                            });
                        }
                    });
                }
                else if (disappear == "lost-focus")
                {
                    $(parent).focusout(function ()
                    {
                        tip.fadeOut(speed, function ()
                        {
                            if (!!onclose)    onclose({parent: parent, target: tip[0]});
                            $(parent).unbind("focusout");
                        });
                    });
                }
            });
        },

        close: function ()
        {
            var tip = $("#" + this.id);
            var parent = this.parent;
            var onclose = this.options.onclose;

            tip.fadeOut(this.options.speed, function ()
            {
                if (!!onclose)    onclose({parent: parent, target: tip[0]});
            });
        }
    };

    $.fn.easytip = function (options)
    {
        var tip = new _easytip(this, options);

        return tip.init();
    };

})(jQuery, window, document);
