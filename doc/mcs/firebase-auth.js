'use strict';
!function(canCreateDiscussions, definition) {
  try {
    definition = definition && definition.hasOwnProperty("default") ? definition.default : definition;
    var e = "undefined" != typeof window ? window : "undefined" != typeof global ? global : "undefined" != typeof self ? self : {};
    (function() {
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function isString(obj) {
        return "string" == typeof obj;
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function translate(obj) {
        return "boolean" == typeof obj;
      }
      /**
       * @return {undefined}
       */
      function noop() {
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function typeOf(value) {
        /** @type {string} */
        var type = typeof value;
        if ("object" == type) {
          if (!value) {
            return "null";
          }
          if (value instanceof Array) {
            return "array";
          }
          if (value instanceof Object) {
            return type;
          }
          /** @type {string} */
          var oldCondition = Object.prototype.toString.call(value);
          if ("[object Window]" == oldCondition) {
            return "object";
          }
          if ("[object Array]" == oldCondition || "number" == typeof value.length && void 0 !== value.splice && void 0 !== value.propertyIsEnumerable && !value.propertyIsEnumerable("splice")) {
            return "array";
          }
          if ("[object Function]" == oldCondition || void 0 !== value.call && void 0 !== value.propertyIsEnumerable && !value.propertyIsEnumerable("call")) {
            return "function";
          }
        } else {
          if ("function" == type && void 0 === value.call) {
            return "object";
          }
        }
        return type;
      }
      /**
       * @param {!Object} key
       * @return {?}
       */
      function processRelativeTime(key) {
        return null === key;
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function each(value) {
        return "array" == typeOf(value);
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function isArray(obj) {
        var type = typeOf(obj);
        return "array" == type || "object" == type && "number" == typeof obj.length;
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function isNaN(obj) {
        return "function" == typeOf(obj);
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function isFunction(value) {
        /** @type {string} */
        var type = typeof value;
        return "object" == type && null != value || "function" == type;
      }
      /**
       * @param {!Function} callback
       * @param {!Object} handler
       * @param {boolean} capture
       * @return {?}
       */
      function addEventListener(callback, handler, capture) {
        return callback.call.apply(callback.bind, arguments);
      }
      /**
       * @param {!Function} source
       * @param {!Object} name
       * @param {boolean} data
       * @return {?}
       */
      function analyze(source, name, data) {
        if (!source) {
          throw Error();
        }
        if (2 < arguments.length) {
          /** @type {!Array<?>} */
          var a = Array.prototype.slice.call(arguments, 2);
          return function() {
            /** @type {!Array<?>} */
            var b = Array.prototype.slice.call(arguments);
            return Array.prototype.unshift.apply(b, a), source.apply(name, b);
          };
        }
        return function() {
          return source.apply(name, arguments);
        };
      }
      /**
       * @param {!Function} e
       * @param {!Object} source
       * @param {boolean} n
       * @return {?}
       */
      function bind(e, source, n) {
        return (bind = Function.prototype.bind && -1 != Function.prototype.bind.toString().indexOf("native code") ? addEventListener : analyze).apply(null, arguments);
      }
      /**
       * @param {!Function} action
       * @param {number} id
       * @return {?}
       */
      function listen(action, id) {
        /** @type {!Array<?>} */
        var n = Array.prototype.slice.call(arguments, 1);
        return function() {
          /** @type {!Array<?>} */
          var args = n.slice();
          return args.push.apply(args, arguments), action.apply(this, args);
        };
      }
      /**
       * @param {!Function} obj
       * @param {!Function} type
       * @return {undefined}
       */
      function expect(obj, type) {
        /**
         * @return {undefined}
         */
        function f() {
        }
        f.prototype = type.prototype;
        obj.lb = type.prototype;
        obj.prototype = new f;
        /** @type {!Function} */
        obj.prototype.constructor = obj;
        /**
         * @param {?} ast
         * @param {?} value
         * @param {?} key
         * @return {?}
         */
        obj.ad = function(ast, value, key) {
          /** @type {!Array} */
          var args = Array(arguments.length - 2);
          /** @type {number} */
          var i = 2;
          for (; i < arguments.length; i++) {
            args[i - 2] = arguments[i];
          }
          return type.prototype[value].apply(ast, args);
        };
      }
      /**
       * @param {!Function} value
       * @return {undefined}
       */
      function defineProperty(value) {
        value.prototype.then = value.prototype.then;
        /** @type {boolean} */
        value.prototype.$goog_Thenable = true;
      }
      /**
       * @param {?} object
       * @return {?}
       */
      function match(object) {
        if (!object) {
          return false;
        }
        try {
          return !!object.$goog_Thenable;
        } catch (t) {
          return false;
        }
      }
      /**
       * @param {?} message
       * @return {undefined}
       */
      function Constructor(message) {
        if (Error.captureStackTrace) {
          Error.captureStackTrace(this, Constructor);
        } else {
          /** @type {string} */
          var stack = Error().stack;
          if (stack) {
            /** @type {string} */
            this.stack = stack;
          }
        }
        if (message) {
          /** @type {string} */
          this.message = String(message);
        }
      }
      /**
       * @param {string} value
       * @param {!NodeList} list
       * @return {undefined}
       */
      function Group(value, list) {
        /** @type {string} */
        var key = "";
        /** @type {number} */
        var i = (value = value.split("%s")).length - 1;
        /** @type {number} */
        var j = 0;
        for (; j < i; j++) {
          /** @type {string} */
          key = key + (value[j] + (j < list.length ? list[j] : "%s"));
        }
        Constructor.call(this, key + value[i]);
      }
      /**
       * @param {string} sort
       * @param {?} array
       * @return {?}
       */
      function sort(sort, array) {
        throw new Group("Failure" + (sort ? ": " + sort : ""), Array.prototype.slice.call(arguments, 1));
      }
      /**
       * @param {number} c
       * @param {!Function} f
       * @return {undefined}
       */
      function Matrix2D(c, f) {
        /** @type {number} */
        this.c = c;
        /** @type {!Function} */
        this.f = f;
        /** @type {number} */
        this.b = 0;
        /** @type {null} */
        this.a = null;
      }
      /**
       * @param {!Object} a
       * @param {!Object} obj
       * @return {undefined}
       */
      function copy(a, obj) {
        a.f(obj);
        if (100 > a.b) {
          a.b++;
          obj.next = a.a;
          /** @type {!Object} */
          a.a = obj;
        }
      }
      /**
       * @return {undefined}
       */
      function Ebur128() {
        /** @type {null} */
        this.b = this.a = null;
      }
      /**
       * @return {?}
       */
      function decode() {
        var t = clist;
        /** @type {null} */
        var a = null;
        return t.a && (a = t.a, t.a = t.a.next, t.a || (t.b = null), a.next = null), a;
      }
      /**
       * @return {undefined}
       */
      function Controller() {
        /** @type {null} */
        this.next = this.b = this.a = null;
      }
      /**
       * @param {!Array} data
       * @param {string} index
       * @return {?}
       */
      function push(data, index) {
        return 0 <= substr(data, index);
      }
      /**
       * @param {!Arguments} path
       * @param {string} value
       * @return {?}
       */
      function lookup(path, value) {
        var handler;
        return (handler = 0 <= (value = substr(path, value))) && Array.prototype.splice.call(path, value, 1), handler;
      }
      /**
       * @param {!Object} a
       * @param {!Function} b
       * @return {undefined}
       */
      function s(a, b) {
        !function(path, fn) {
          var i = path.length;
          var keys = isString(path) ? path.split("") : path;
          --i;
          for (; 0 <= i; --i) {
            if (i in keys) {
              fn.call(void 0, keys[i], i, path);
            }
          }
        }(a, function(deferBuild, right) {
          if (b.call(void 0, deferBuild, right, a) && 1 == Array.prototype.splice.call(a, right, 1).length) {
            0;
          }
        });
      }
      /**
       * @param {?} n
       * @return {?}
       */
      function F(n) {
        return Array.prototype.concat.apply([], arguments);
      }
      /**
       * @param {!NodeList} body
       * @return {?}
       */
      function require(body) {
        var length = body.length;
        if (0 < length) {
          /** @type {!Array} */
          var result = Array(length);
          /** @type {number} */
          var i = 0;
          for (; i < length; i++) {
            result[i] = body[i];
          }
          return result;
        }
        return [];
      }
      /**
       * @param {string} add
       * @param {string} src
       * @return {?}
       */
      function extend(add, src) {
        var headersAndBody = add.split("%s");
        /** @type {string} */
        var pix_color = "";
        /** @type {!Array<?>} */
        var keysToSend = Array.prototype.slice.call(arguments, 1);
        for (; keysToSend.length && 1 < headersAndBody.length;) {
          /** @type {string} */
          pix_color = pix_color + (headersAndBody.shift() + keysToSend.shift());
        }
        return pix_color + headersAndBody.join("%s");
      }
      /**
       * @param {string} string
       * @return {?}
       */
      function createNode(string) {
        return regBigBrackets.test(string) ? (-1 != string.indexOf("&") && (string = string.replace(refLinkIDRegex, "&amp;")), -1 != string.indexOf("<") && (string = string.replace(spanOpenOrCloseRegExp, "&lt;")), -1 != string.indexOf(">") && (string = string.replace(regexSeparators, "&gt;")), -1 != string.indexOf('"') && (string = string.replace(json_escapable, "&quot;")), -1 != string.indexOf("'") && (string = string.replace(ESCAPE, "&#39;")), -1 != string.indexOf("\x00") && (string = string.replace(reA, 
        "&#0;")), string) : string;
      }
      /**
       * @param {!Object} s
       * @param {string} name
       * @return {?}
       */
      function has(s, name) {
        return -1 != s.indexOf(name);
      }
      /**
       * @param {number} progressOld
       * @param {number} progressNew
       * @return {?}
       */
      function exists(progressOld, progressNew) {
        return progressOld < progressNew ? -1 : progressOld > progressNew ? 1 : 0;
      }
      /**
       * @param {string} str
       * @return {?}
       */
      function floor(str) {
        return has(v, str);
      }
      /**
       * @param {!Object} data
       * @param {!Function} callback
       * @return {undefined}
       */
      function attr(data, callback) {
        var i;
        for (i in data) {
          callback.call(void 0, data[i], i, data);
        }
      }
      /**
       * @param {?} value
       * @return {?}
       */
      function isMatch(value) {
        var fqPropertyName;
        for (fqPropertyName in value) {
          return false;
        }
        return true;
      }
      /**
       * @param {!Array} value
       * @return {?}
       */
      function clamp(value) {
        var k;
        var x = {};
        for (k in value) {
          x[k] = value[k];
        }
        return x;
      }
      /**
       * @param {!Array} s
       * @param {?} value
       * @return {undefined}
       */
      function log(s, value) {
//         var prop;
        var props;
        /** @type {number} */
        var i = 1;
        for (; i < arguments.length; i++) {
          for (prop in props = arguments[i]) {
            s[prop] = props[prop];
          }
          /** @type {number} */
          var i = 0;
          for (; i < intFields.length; i++) {
            /** @type {string} */
            prop = intFields[i];
            if (Object.prototype.hasOwnProperty.call(props, prop)) {
              s[prop] = props[prop];
            }
          }
        }
      }
      /**
       * @param {?} text
       * @return {undefined}
       */
      function del(text) {
        global.setTimeout(function() {
          throw text;
        }, 0);
      }
      /**
       * @param {string} node
       * @param {(Object|string)} item
       * @return {undefined}
       */
      function wrapper(node, item) {
        if (!open) {
          (function() {
            if (global.Promise && global.Promise.resolve) {
              var comingTracks = global.Promise.resolve(void 0);
              /**
               * @return {undefined}
               */
              open = function() {
                comingTracks.then(task);
              };
            } else {
              /**
               * @return {undefined}
               */
              open = function() {
                /** @type {function(): undefined} */
                var fn = task;
                if (!isNaN(global.setImmediate) || global.Window && global.Window.prototype && !floor("Edge") && global.Window.prototype.setImmediate == global.setImmediate) {
                  if (!openScorm) {
                    openScorm = function() {
                      var Channel = global.MessageChannel;
                      if (void 0 === Channel && "undefined" != typeof window && window.postMessage && window.addEventListener && !floor("Presto") && (Channel = function() {
                        /** @type {!Element} */
                        var t = document.createElement("IFRAME");
                        /** @type {string} */
                        t.style.display = "none";
                        /** @type {string} */
                        t.src = "";
                        document.documentElement.appendChild(t);
                        var window = t.contentWindow;
                        (t = window.document).open();
                        t.write("");
                        t.close();
                        /** @type {string} */
                        var message = "callImmediate" + Math.random();
                        /** @type {string} */
                        var args = "file:" == window.location.protocol ? "*" : window.location.protocol + "//" + window.location.host;
                        t = bind(function(resp) {
                          if (!("*" != args && resp.origin != args || resp.data != message)) {
                            this.port1.onmessage();
                          }
                        }, this);
                        window.addEventListener("message", t, false);
                        this.port1 = {};
                        this.port2 = {
                          postMessage : function() {
                            window.postMessage(message, args);
                          }
                        };
                      }), void 0 !== Channel && !floor("Trident") && !floor("MSIE")) {
                        var channel = new Channel;
                        var m = {};
                        var cur = m;
                        return channel.port1.onmessage = function() {
                          if (void 0 !== m.next) {
                            var callback_send_audio_to_audio_player = (m = m.next).rb;
                            /** @type {null} */
                            m.rb = null;
                            callback_send_audio_to_audio_player();
                          }
                        }, function(common) {
                          cur.next = {
                            rb : common
                          };
                          cur = cur.next;
                          channel.port2.postMessage(0);
                        };
                      }
                      return "undefined" != typeof document && "onreadystatechange" in document.createElement("SCRIPT") ? function(notify_success) {
                        /** @type {!Element} */
                        var script = document.createElement("SCRIPT");
                        /**
                         * @return {undefined}
                         */
                        script.onreadystatechange = function() {
                          /** @type {null} */
                          script.onreadystatechange = null;
                          script.parentNode.removeChild(script);
                          /** @type {null} */
                          script = null;
                          notify_success();
                          /** @type {null} */
                          notify_success = null;
                        };
                        document.documentElement.appendChild(script);
                      } : function(fn) {
                        global.setTimeout(fn, 0);
                      };
                    }();
                  }
                  openScorm(fn);
                } else {
                  global.setImmediate(fn);
                }
              };
            }
          })();
        }
        if (!dt) {
          open();
          /** @type {boolean} */
          dt = true;
        }
        clist.add(node, item);
      }
      /**
       * @return {undefined}
       */
      function task() {
        var value;
        for (; value = decode();) {
          try {
            value.a.call(value.b);
          } catch (Link) {
            del(Link);
          }
          copy(_context, value);
        }
        /** @type {boolean} */
        dt = false;
      }
      /**
       * @param {?} name
       * @param {?} fsm
       * @return {undefined}
       */
      function Promise(name, fsm) {
        if (this.a = image, this.j = void 0, this.f = this.b = this.c = null, this.g = this.h = false, name != noop) {
          try {
            var effect = this;
            name.call(fsm, function(url) {
              handler(effect, feature, url);
            }, function(obj) {
              if (!(obj instanceof Action)) {
                try {
                  if (obj instanceof Error) {
                    throw obj;
                  }
                  throw Error("Promise rejected.");
                } catch (t) {
                }
              }
              handler(effect, object, obj);
            });
          } catch (reason) {
            handler(this, object, reason);
          }
        }
      }
      /**
       * @return {undefined}
       */
      function RunHandlerTask() {
        /** @type {null} */
        this.next = this.f = this.b = this.g = this.a = null;
        /** @type {boolean} */
        this.c = false;
      }
      /**
       * @param {string} v
       * @param {string} data
       * @param {!Function} value
       * @return {?}
       */
      function write(v, data, value) {
        var cell = search.get();
        return cell.g = v, cell.b = data, cell.f = value, cell;
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function resolve(value) {
        if (value instanceof Promise) {
          return value;
        }
        var result = new Promise(noop);
        return handler(result, feature, value), result;
      }
      /**
       * @param {?} value
       * @return {?}
       */
      function cb(value) {
        return new Promise(function(canCreateDiscussions, __) {
          __(value);
        });
      }
      /**
       * @param {undefined} key
       * @param {undefined} opts
       * @param {!Function} callback
       * @return {undefined}
       */
      function compose(key, opts, callback) {
        if (!animate(key, opts, callback, null)) {
          wrapper(listen(opts, key));
        }
      }
      /**
       * @param {!Object} options
       * @param {!Function} value
       * @return {undefined}
       */
      function forEach(options, value) {
        if (!(options.b || options.a != feature && options.a != object)) {
          execute(options);
        }
        if (options.f) {
          /** @type {!Function} */
          options.f.next = value;
        } else {
          /** @type {!Function} */
          options.b = value;
        }
        /** @type {!Function} */
        options.f = value;
      }
      /**
       * @param {string} obj
       * @param {!Function} val
       * @param {!Function} type
       * @param {!Object} res
       * @return {?}
       */
      function html(obj, val, type, res) {
        var item = write(null, null, null);
        return item.a = new Promise(function(expand, clone) {
          item.g = val ? function(obj) {
            try {
              var item = val.call(res, obj);
              expand(item);
            } catch (t) {
              clone(t);
            }
          } : expand;
          item.b = type ? function(obj) {
            try {
              var item = type.call(res, obj);
              if (void 0 === item && obj instanceof Action) {
                clone(obj);
              } else {
                expand(item);
              }
            } catch (n) {
              clone(n);
            }
          } : clone;
        }), item.a.c = obj, forEach(obj, item), item.a;
      }
      /**
       * @param {!Object} options
       * @param {number} type
       * @param {?} arg
       * @return {undefined}
       */
      function handler(options, type, arg) {
        if (options.a == image) {
          if (options === arg) {
            /** @type {number} */
            type = object;
            /** @type {!TypeError} */
            arg = new TypeError("Promise cannot resolve to itself");
          }
          /** @type {number} */
          options.a = 1;
          if (!animate(arg, options.Kc, options.Lc, options)) {
            options.j = arg;
            /** @type {number} */
            options.a = type;
            /** @type {null} */
            options.c = null;
            execute(options);
            if (!(type != object || arg instanceof Action)) {
              (function(domFixtures, e) {
                /** @type {boolean} */
                domFixtures.g = true;
                wrapper(function() {
                  if (domFixtures.g) {
                    onBodyKeyup.call(null, e);
                  }
                });
              })(options, arg);
            }
          }
        }
      }
      /**
       * @param {!Object} obj
       * @param {string} done
       * @param {!Function} fn
       * @param {!Object} callback
       * @return {?}
       */
      function animate(obj, done, fn, callback) {
        if (obj instanceof Promise) {
          return forEach(obj, write(done || noop, fn || null, callback)), true;
        }
        if (match(obj)) {
          return obj.then(done, fn, callback), true;
        }
        if (isFunction(obj)) {
          try {
            var c = obj.then;
            if (isNaN(c)) {
              return function(instance, module, s, handler, next) {
                /**
                 * @param {?} val
                 * @return {undefined}
                 */
                function stop(val) {
                  if (!a) {
                    /** @type {boolean} */
                    a = true;
                    handler.call(next, val);
                  }
                }
                /** @type {boolean} */
                var a = false;
                try {
                  module.call(instance, function(family) {
                    if (!a) {
                      /** @type {boolean} */
                      a = true;
                      s.call(next, family);
                    }
                  }, stop);
                } catch (step) {
                  stop(step);
                }
              }(obj, c, done, fn, callback), true;
            }
          } catch (event) {
            return fn.call(callback, event), true;
          }
        }
        return false;
      }
      /**
       * @param {!Object} x
       * @return {undefined}
       */
      function execute(x) {
        if (!x.h) {
          /** @type {boolean} */
          x.h = true;
          wrapper(x.Ub, x);
        }
      }
      /**
       * @param {!Object} r
       * @return {?}
       */
      function calcPoint(r) {
        /** @type {null} */
        var a = null;
        return r.b && (a = r.b, r.b = a.next, a.next = null), r.b || (r.f = null), a;
      }
      /**
       * @param {!Object} node
       * @param {!Object} value
       * @param {number} s
       * @param {?} data
       * @return {undefined}
       */
      function e(node, value, s, data) {
        if (s == object && value.b && !value.c) {
          for (; node && node.g; node = node.c) {
            /** @type {boolean} */
            node.g = false;
          }
        }
        if (value.a) {
          /** @type {null} */
          value.a.c = null;
          prepend(value, s, data);
        } else {
          try {
            if (value.c) {
              value.g.call(value.f);
            } else {
              prepend(value, s, data);
            }
          } catch (e) {
            onBodyKeyup.call(null, e);
          }
        }
        copy(search, value);
      }
      /**
       * @param {!Object} value
       * @param {number} name
       * @param {?} tweet
       * @return {undefined}
       */
      function prepend(value, name, tweet) {
        if (name == feature) {
          value.g.call(value.f, tweet);
        } else {
          if (value.b) {
            value.b.call(value.f, tweet);
          }
        }
      }
      /**
       * @param {?} args
       * @return {undefined}
       */
      function Action(args) {
        Constructor.call(this, args);
      }
      /**
       * @return {undefined}
       */
      function method() {
        if (0 != disconnected) {
          nextIdLookup[this[eventIDBase] || (this[eventIDBase] = ++eventIDCount)] = this;
        }
        this.pa = this.pa;
        this.oa = this.oa;
      }
      /**
       * @param {!Object} self
       * @return {undefined}
       */
      function exec(self) {
        if (!self.pa && (self.pa = true, self.ua(), 0 != disconnected)) {
          var indexLookupKey = self[eventIDBase] || (self[eventIDBase] = ++eventIDCount);
          if (0 != disconnected && self.oa && 0 < self.oa.length) {
            throw Error(self + " did not empty its onDisposeCallbacks queue. This probably means it overrode dispose() or disposeInternal() without calling the superclass' method.");
          }
          delete nextIdLookup[indexLookupKey];
        }
      }
      /**
       * @param {?} wsdlTypes
       * @return {?}
       */
      function _getTypeFromWsdl(wsdlTypes) {
        return _getTypeFromWsdl[" "](wsdlTypes), wsdlTypes;
      }
      /**
       * @return {?}
       */
      function factory() {
        var document = global.document;
        return document ? document.documentMode : void 0;
      }
      /**
       * @param {!Object} i
       * @return {?}
       */
      function apply(i) {
        return function(name, require) {
          var target = descmain;
          return Object.prototype.hasOwnProperty.call(target, name) ? target[name] : target[name] = require(name);
        }(i, function() {
          /** @type {number} */
          var horizontal = 0;
          var n = escapeHtml(String(expected)).split(".");
          var l = escapeHtml(String(i)).split(".");
          /** @type {number} */
          var numBuckets = Math.max(n.length, l.length);
          /** @type {number} */
          var x = 0;
          for (; 0 == horizontal && x < numBuckets; x++) {
            var value = n[x] || "";
            var url = l[x] || "";
            do {
              if (value = /(\d*)(\D*)(.*)/.exec(value) || ["", "", "", ""], url = /(\d*)(\D*)(.*)/.exec(url) || ["", "", "", ""], 0 == value[0].length && 0 == url[0].length) {
                break;
              }
              horizontal = exists(0 == value[1].length ? 0 : parseInt(value[1], 10), 0 == url[1].length ? 0 : parseInt(url[1], 10)) || exists(0 == value[2].length, 0 == url[2].length) || exists(value[2], url[2]);
              value = value[3];
              url = url[3];
            } while (0 == horizontal);
          }
          return 0 <= horizontal;
        });
      }
      /**
       * @param {!Object} type
       * @param {string} target
       * @return {undefined}
       */
      function Event(type, target) {
        /** @type {!Object} */
        this.type = type;
        this.b = this.target = target;
        /** @type {boolean} */
        this.Eb = true;
      }
      /**
       * @param {!Object} e
       * @param {number} target
       * @return {undefined}
       */
      function start(e, target) {
        if (Event.call(this, e ? e.type : ""), this.relatedTarget = this.b = this.target = null, this.button = this.screenY = this.screenX = this.clientY = this.clientX = 0, this.key = "", this.metaKey = this.shiftKey = this.altKey = this.ctrlKey = false, this.pointerId = 0, this.pointerType = "", this.a = null, e) {
          var type = this.type = e.type;
          var relevantTouch = e.changedTouches ? e.changedTouches[0] : null;
          if (this.target = e.target || e.srcElement, this.b = target, target = e.relatedTarget) {
            if (visible) {
              t: {
                try {
                  _getTypeFromWsdl(target.nodeName);
                  /** @type {boolean} */
                  var r = true;
                  break t;
                } catch (t) {
                }
                /** @type {boolean} */
                r = false;
              }
              if (!r) {
                /** @type {null} */
                target = null;
              }
            }
          } else {
            if ("mouseover" == type) {
              target = e.fromElement;
            } else {
              if ("mouseout" == type) {
                target = e.toElement;
              }
            }
          }
          /** @type {number} */
          this.relatedTarget = target;
          if (null === relevantTouch) {
            this.clientX = void 0 !== e.clientX ? e.clientX : e.pageX;
            this.clientY = void 0 !== e.clientY ? e.clientY : e.pageY;
            this.screenX = e.screenX || 0;
            this.screenY = e.screenY || 0;
          } else {
            this.clientX = void 0 !== relevantTouch.clientX ? relevantTouch.clientX : relevantTouch.pageX;
            this.clientY = void 0 !== relevantTouch.clientY ? relevantTouch.clientY : relevantTouch.pageY;
            this.screenX = relevantTouch.screenX || 0;
            this.screenY = relevantTouch.screenY || 0;
          }
          this.button = e.button;
          this.key = e.key || "";
          this.ctrlKey = e.ctrlKey;
          this.altKey = e.altKey;
          this.shiftKey = e.shiftKey;
          this.metaKey = e.metaKey;
          this.pointerId = e.pointerId || 0;
          this.pointerType = isString(e.pointerType) ? e.pointerType : MSPOINTER_TYPES[e.pointerType] || "";
          /** @type {!Object} */
          this.a = e;
          if (e.defaultPrevented) {
            this.preventDefault();
          }
        }
      }
      /**
       * @param {!Object} params
       * @return {undefined}
       */
      function abs(params) {
        /** @type {boolean} */
        params.ma = true;
        /** @type {null} */
        params.listener = null;
        /** @type {null} */
        params.proxy = null;
        /** @type {null} */
        params.src = null;
        /** @type {null} */
        params.La = null;
      }
      /**
       * @param {string} data
       * @return {undefined}
       */
      function Model(data) {
        /** @type {string} */
        this.src = data;
        this.a = {};
        /** @type {number} */
        this.b = 0;
      }
      /**
       * @param {!Object} elem
       * @param {!Object} value
       * @return {undefined}
       */
      function getType(elem, value) {
        var type = value.type;
        if (type in elem.a && lookup(elem.a[type], value)) {
          abs(value);
          if (0 == elem.a[type].length) {
            delete elem.a[type];
            elem.b--;
          }
        }
      }
      /**
       * @param {!Object} options
       * @param {!Object} cb
       * @param {!Object} model
       * @param {!Array} items
       * @return {?}
       */
      function iterator(options, cb, model, items) {
        /** @type {number} */
        var i = 0;
        for (; i < options.length; ++i) {
          var params = options[i];
          if (!params.ma && params.listener == cb && params.capture == !!model && params.La == items) {
            return i;
          }
        }
        return -1;
      }
      /**
       * @param {!Object} object
       * @param {string} name
       * @param {!Object} cb
       * @param {!Object} value
       * @param {!Array} options
       * @return {undefined}
       */
      function map(object, name, cb, value, options) {
        if (value && value.once) {
          send(object, name, cb, value, options);
        } else {
          if (each(name)) {
            /** @type {number} */
            var i = 0;
            for (; i < name.length; i++) {
              map(object, name[i], cb, value, options);
            }
          } else {
            cb = once(cb);
            if (object && object[key]) {
              register(object, name, cb, isFunction(value) ? !!value.capture : !!value, options);
            } else {
              on(object, name, cb, false, value, options);
            }
          }
        }
      }
      /**
       * @param {!Object} t
       * @param {string} e
       * @param {!Object} c
       * @param {!Object} d
       * @param {?} opts
       * @param {!Array} params
       * @return {undefined}
       */
      function on(t, e, c, d, opts, params) {
        if (!e) {
          throw Error("Invalid event type");
        }
        /** @type {boolean} */
        var data = isFunction(opts) ? !!opts.capture : !!opts;
        var m = moment(t);
        if (m || (t[part] = m = new Model(t)), !(c = m.add(e, c, d, data, params)).proxy) {
          if (d = function() {
            /** @type {function(!Object, string): ?} */
            var assert = exports;
            /** @type {function(?): ?} */
            var listener = W3C_MODEL ? function(n) {
              return assert.call(listener.src, listener.listener, n);
            } : function(props) {
              if (!(props = assert.call(listener.src, listener.listener, props))) {
                return props;
              }
            };
            return listener;
          }(), c.proxy = d, d.src = t, d.listener = c, t.addEventListener) {
            if (!ae) {
              /** @type {boolean} */
              opts = data;
            }
            if (void 0 === opts) {
              /** @type {boolean} */
              opts = false;
            }
            t.addEventListener(e.toString(), d, opts);
          } else {
            if (t.attachEvent) {
              t.attachEvent(capitalize(e.toString()), d);
            } else {
              if (!t.addListener || !t.removeListener) {
                throw Error("addEventListener and attachEvent are unavailable.");
              }
              t.addListener(d);
            }
          }
        }
      }
      /**
       * @param {!Object} val
       * @param {string} name
       * @param {!Object} done
       * @param {!Object} value
       * @param {!Array} next
       * @return {undefined}
       */
      function send(val, name, done, value, next) {
        if (each(name)) {
          /** @type {number} */
          var i = 0;
          for (; i < name.length; i++) {
            send(val, name[i], done, value, next);
          }
        } else {
          done = once(done);
          if (val && val[key]) {
            append(val, name, done, isFunction(value) ? !!value.capture : !!value, next);
          } else {
            on(val, name, done, true, value, next);
          }
        }
      }
      /**
       * @param {string} obj
       * @param {!Object} data
       * @param {number} fn
       * @param {!Object} options
       * @param {!Array} selector
       * @return {undefined}
       */
      function add(obj, data, fn, options, selector) {
        if (each(data)) {
          /** @type {number} */
          var value = 0;
          for (; value < data.length; value++) {
            add(obj, data[value], fn, options, selector);
          }
        } else {
          /** @type {boolean} */
          options = isFunction(options) ? !!options.capture : !!options;
          fn = once(fn);
          if (obj && obj[key]) {
            obj = obj.u;
            if ((data = String(data).toString()) in obj.a) {
              if (-1 < (fn = iterator(value = obj.a[data], fn, options, selector))) {
                abs(value[fn]);
                Array.prototype.splice.call(value, fn, 1);
                if (0 == value.length) {
                  delete obj.a[data];
                  obj.b--;
                }
              }
            }
          } else {
            if (obj && (obj = moment(obj))) {
              data = obj.a[data.toString()];
              /** @type {number} */
              obj = -1;
              if (data) {
                obj = iterator(data, fn, options, selector);
              }
              if (fn = -1 < obj ? data[obj] : null) {
                loop(fn);
              }
            }
          }
        }
      }
      /**
       * @param {!Object} value
       * @return {undefined}
       */
      function loop(value) {
        if ("number" != typeof value && value && !value.ma) {
          var item = value.src;
          if (item && item[key]) {
            getType(item.u, value);
          } else {
            var type = value.type;
            var callback = value.proxy;
            if (item.removeEventListener) {
              item.removeEventListener(type, callback, value.capture);
            } else {
              if (item.detachEvent) {
                item.detachEvent(capitalize(type), callback);
              } else {
                if (item.addListener && item.removeListener) {
                  item.removeListener(callback);
                }
              }
            }
            if (type = moment(item)) {
              getType(type, value);
              if (0 == type.b) {
                /** @type {null} */
                type.src = null;
                /** @type {null} */
                item[part] = null;
              }
            } else {
              abs(value);
            }
          }
        }
      }
      /**
       * @param {string} type
       * @return {?}
       */
      function capitalize(type) {
        return type in appenderMap ? appenderMap[type] : appenderMap[type] = "on" + type;
      }
      /**
       * @param {!Object} b
       * @param {!Object} e
       * @param {boolean} search
       * @param {string} name
       * @return {?}
       */
      function matches(b, e, search, name) {
        /** @type {boolean} */
        var success = true;
        if ((b = moment(b)) && (e = b.a[e.toString()])) {
          e = e.concat();
          /** @type {number} */
          b = 0;
          for (; b < e.length; b++) {
            var o = e[b];
            if (o && o.capture == search && !o.ma) {
              o = destroy(o, name);
              /** @type {boolean} */
              success = success && false !== o;
            }
          }
        }
        return success;
      }
      /**
       * @param {!Object} data
       * @param {string} e
       * @return {?}
       */
      function destroy(data, e) {
        var listenerFn = data.listener;
        var listenerHandler = data.La || data.src;
        return data.Ha && loop(data), listenerFn.call(listenerHandler, e);
      }
      /**
       * @param {!Object} name
       * @param {string} args
       * @return {?}
       */
      function exports(name, args) {
        if (name.ma) {
          return true;
        }
        if (!W3C_MODEL) {
          if (!args) {
            t: {
              /** @type {!Array} */
              args = ["window", "event"];
              var res = global;
              /** @type {number} */
              var a = 0;
              for (; a < args.length; a++) {
                if (null == (res = res[args[a]])) {
                  /** @type {null} */
                  args = null;
                  break t;
                }
              }
              args = res;
            }
          }
          if (args = new start(a = args, this), res = true, !(0 > a.keyCode || void 0 != a.returnValue)) {
            t: {
              /** @type {boolean} */
              var i = false;
              if (0 == a.keyCode) {
                try {
                  /** @type {number} */
                  a.keyCode = -1;
                  break t;
                } catch (t) {
                  /** @type {boolean} */
                  i = true;
                }
              }
              if (i || void 0 == a.returnValue) {
                /** @type {boolean} */
                a.returnValue = true;
              }
            }
            /** @type {!Array} */
            a = [];
            i = args.b;
            for (; i; i = i.parentNode) {
              a.push(i);
            }
            name = name.type;
            /** @type {number} */
            i = a.length - 1;
            for (; 0 <= i; i--) {
              args.b = a[i];
              var result = matches(a[i], name, true, args);
              res = res && result;
            }
            /** @type {number} */
            i = 0;
            for (; i < a.length; i++) {
              args.b = a[i];
              result = matches(a[i], name, false, args);
              res = res && result;
            }
          }
          return res;
        }
        return destroy(name, new start(args, this));
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function moment(obj) {
        return (obj = obj[part]) instanceof Model ? obj : null;
      }
      /**
       * @param {!Object} el
       * @return {?}
       */
      function once(el) {
        return isNaN(el) ? el : (el[CLIENT_TOP] || (el[CLIENT_TOP] = function(event) {
          return el.handleEvent(event);
        }), el[CLIENT_TOP]);
      }
      /**
       * @return {undefined}
       */
      function p() {
        method.call(this);
        this.u = new Model(this);
        this.Mb = this;
        /** @type {null} */
        this.Ta = null;
      }
      /**
       * @param {!Object} target
       * @param {string} type
       * @param {string} fn
       * @param {string} once
       * @param {!Array} callback
       * @return {undefined}
       */
      function register(target, type, fn, once, callback) {
        target.u.add(String(type), fn, false, once, callback);
      }
      /**
       * @param {!Object} obj
       * @param {string} name
       * @param {string} block
       * @param {string} message
       * @param {!Array} callback
       * @return {undefined}
       */
      function append(obj, name, block, message, callback) {
        obj.u.add(String(name), block, true, message, callback);
      }
      /**
       * @param {!Object} v
       * @param {!Array} obj
       * @param {boolean} name
       * @param {!Object} value
       * @return {?}
       */
      function invoke(v, obj, name, value) {
        if (!(obj = v.u.a[String(obj)])) {
          return true;
        }
        obj = obj.concat();
        /** @type {boolean} */
        var result = true;
        /** @type {number} */
        var i = 0;
        for (; i < obj.length; ++i) {
          var item = obj[i];
          if (item && !item.ma && item.capture == name) {
            var callback = item.listener;
            var $sections = item.La || item.src;
            if (item.Ha) {
              getType(v.u, item);
            }
            /** @type {boolean} */
            result = false !== callback.call($sections, value) && result;
          }
        }
        return result && 0 != value.Eb;
      }
      /**
       * @param {!Object} t
       * @param {number} time
       * @param {!Object} value
       * @return {?}
       */
      function filter(t, time, value) {
        if (isNaN(t)) {
          if (value) {
            t = bind(t, value);
          }
        } else {
          if (!t || "function" != typeof t.handleEvent) {
            throw Error("Invalid listener argument");
          }
          t = bind(t.handleEvent, t);
        }
        return 2147483647 < Number(time) ? -1 : global.setTimeout(t, time || 0);
      }
      /**
       * @param {number} i
       * @return {?}
       */
      function validate(i) {
        /** @type {null} */
        var val = null;
        return (new Promise(function(saveNotifs, callback) {
          if (-1 == (val = filter(function() {
            saveNotifs(void 0);
          }, i))) {
            callback(Error("Failed to schedule timer."));
          }
        })).m(function(acc) {
          throw global.clearTimeout(val), acc;
        });
      }
      /**
       * @param {!Object} data
       * @return {?}
       */
      function split(data) {
        if (data.S && "function" == typeof data.S) {
          return data.S();
        }
        if (isString(data)) {
          return data.split("");
        }
        if (isArray(data)) {
          /** @type {!Array} */
          var result = [];
          var n = data.length;
          /** @type {number} */
          var i = 0;
          for (; i < n; i++) {
            result.push(data[i]);
          }
          return result;
        }
        for (i in result = [], n = 0, data) {
          result[n++] = data[i];
        }
        return result;
      }
      /**
       * @param {string} o
       * @return {?}
       */
      function reduce(o) {
        if (o.U && "function" == typeof o.U) {
          return o.U();
        }
        if (!o.S || "function" != typeof o.S) {
          if (isArray(o) || isString(o)) {
            /** @type {!Array} */
            var rv = [];
            o = o.length;
            /** @type {number} */
            var i = 0;
            for (; i < o; i++) {
              rv.push(i);
            }
            return rv;
          }
          var beknehfpfkghjoafdifaflglpjkojoco;
          for (beknehfpfkghjoafdifaflglpjkojoco in rv = [], i = 0, o) {
            /** @type {string} */
            rv[i++] = beknehfpfkghjoafdifaflglpjkojoco;
          }
          return rv;
        }
      }
      /**
       * @param {!Object} value
       * @param {?} features
       * @return {undefined}
       */
      function Vector(value, features) {
        this.b = {};
        /** @type {!Array} */
        this.a = [];
        /** @type {number} */
        this.c = 0;
        /** @type {number} */
        var m = arguments.length;
        if (1 < m) {
          if (m % 2) {
            throw Error("Uneven number of arguments");
          }
          /** @type {number} */
          var i = 0;
          for (; i < m; i = i + 2) {
            this.set(arguments[i], arguments[i + 1]);
          }
        } else {
          if (value) {
            if (value instanceof Vector) {
              m = value.U();
              /** @type {number} */
              i = 0;
              for (; i < m.length; i++) {
                this.set(m[i], value.get(m[i]));
              }
            } else {
              for (i in value) {
                this.set(i, value[i]);
              }
            }
          }
        }
      }
      /**
       * @param {!Object} options
       * @return {undefined}
       */
      function selectItem(options) {
        if (options.c != options.a.length) {
          /** @type {number} */
          var j = 0;
          /** @type {number} */
          var i = 0;
          for (; j < options.a.length;) {
            var c = options.a[j];
            if (hasOwnProperty(options.b, c)) {
              options.a[i++] = c;
            }
            j++;
          }
          /** @type {number} */
          options.a.length = i;
        }
        if (options.c != options.a.length) {
          var uniq = {};
          /** @type {number} */
          i = j = 0;
          for (; j < options.a.length;) {
            if (!hasOwnProperty(uniq, c = options.a[j])) {
              options.a[i++] = c;
              /** @type {number} */
              uniq[c] = 1;
            }
            j++;
          }
          /** @type {number} */
          options.a.length = i;
        }
      }
      /**
       * @param {?} obj
       * @param {string} prop
       * @return {?}
       */
      function hasOwnProperty(obj, prop) {
        return Object.prototype.hasOwnProperty.call(obj, prop);
      }
      /**
       * @param {!Object} obj
       * @param {number} a
       * @return {undefined}
       */
      function RegExp(obj, a) {
        if (this.b = this.l = this.c = "", this.j = null, this.h = this.g = "", this.f = false, obj instanceof RegExp) {
          this.f = void 0 !== a ? a : obj.f;
          assert(this, obj.c);
          this.l = obj.l;
          this.b = obj.b;
          distance(this, obj.j);
          this.g = obj.g;
          a = obj.a;
          var t = new A;
          t.c = a.c;
          if (a.a) {
            t.a = new Vector(a.a);
            t.b = a.b;
          }
          read(this, t);
          this.h = obj.h;
        } else {
          if (obj && (t = String(obj).match(re2))) {
            /** @type {boolean} */
            this.f = !!a;
            assert(this, t[1] || "", true);
            this.l = getParams(t[2] || "");
            this.b = getParams(t[3] || "", true);
            distance(this, t[4]);
            this.g = getParams(t[5] || "", true);
            read(this, t[6] || "", true);
            this.h = getParams(t[7] || "");
          } else {
            /** @type {boolean} */
            this.f = !!a;
            this.a = new A(null, this.f);
          }
        }
      }
      /**
       * @param {!Object} options
       * @param {string} type
       * @param {boolean} to
       * @return {undefined}
       */
      function assert(options, type, to) {
        options.c = to ? getParams(type, true) : type;
        if (options.c) {
          options.c = options.c.replace(/:$/, "");
        }
      }
      /**
       * @param {!Object} options
       * @param {number} val
       * @return {undefined}
       */
      function distance(options, val) {
        if (val) {
          if (val = Number(val), isNaN(val) || 0 > val) {
            throw Error("Bad port number " + val);
          }
          /** @type {number} */
          options.j = val;
        } else {
          /** @type {null} */
          options.j = null;
        }
      }
      /**
       * @param {!Object} scope
       * @param {string} value
       * @param {boolean} options
       * @return {undefined}
       */
      function read(scope, value, options) {
        if (value instanceof A) {
          /** @type {string} */
          scope.a = value;
          (function(result, i) {
            if (i && !result.f) {
              initialize(result);
              /** @type {null} */
              result.c = null;
              result.a.forEach(function(next, key) {
                var options = key.toLowerCase();
                if (key != options) {
                  _(this, key);
                  submit(this, options, next);
                }
              }, result);
            }
            /** @type {number} */
            result.f = i;
          })(scope.a, scope.f);
        } else {
          if (!options) {
            value = traverse(value, selector);
          }
          scope.a = new A(value, scope.f);
        }
      }
      /**
       * @param {(number|string)} input
       * @param {string} key
       * @param {!Object} n
       * @return {undefined}
       */
      function func(input, key, n) {
        input.a.set(key, n);
      }
      /**
       * @param {(number|string)} val
       * @param {string} obj
       * @return {?}
       */
      function isFinite(val, obj) {
        return val.a.get(obj);
      }
      /**
       * @param {string} value
       * @return {?}
       */
      function getValue(value) {
        return value instanceof RegExp ? new RegExp(value) : new RegExp(value, void 0);
      }
      /**
       * @param {string} n
       * @param {string} c
       * @return {?}
       */
      function setColor(n, c) {
        var t = new RegExp(null, void 0);
        return assert(t, "https"), n && (t.b = n), c && (t.g = c), t;
      }
      /**
       * @param {string} str
       * @param {string} param
       * @return {?}
       */
      function getParams(str, param) {
        return str ? param ? decodeURI(str.replace(/%25/g, "%2525")) : decodeURIComponent(str) : "";
      }
      /**
       * @param {!Object} data
       * @param {!RegExp} path
       * @param {boolean} version
       * @return {?}
       */
      function traverse(data, path, version) {
        return isString(data) ? (data = encodeURI(data).replace(path, replaceCSS), version && (data = data.replace(/%25([0-9a-fA-F]{2})/g, "%$1")), data) : null;
      }
      /**
       * @param {number} selector
       * @return {?}
       */
      function replaceCSS(selector) {
        return "%" + ((selector = selector.charCodeAt(0)) >> 4 & 15).toString(16) + (15 & selector).toString(16);
      }
      /**
       * @param {string} arcLarge
       * @param {?} arcSweep
       * @return {undefined}
       */
      function A(arcLarge, arcSweep) {
        /** @type {null} */
        this.b = this.a = null;
        this.c = arcLarge || null;
        /** @type {boolean} */
        this.f = !!arcSweep;
      }
      /**
       * @param {!Object} v
       * @return {undefined}
       */
      function initialize(v) {
        if (!v.a) {
          v.a = new Vector;
          /** @type {number} */
          v.b = 0;
          if (v.c) {
            (function(h, done) {
              if (h) {
                h = h.split("&");
                /** @type {number} */
                var j = 0;
                for (; j < h.length; j++) {
                  var i = h[j].indexOf("=");
                  /** @type {null} */
                  var value = null;
                  if (0 <= i) {
                    var m = h[j].substring(0, i);
                    value = h[j].substring(i + 1);
                  } else {
                    m = h[j];
                  }
                  done(m, value ? decodeURIComponent(value.replace(/\+/g, " ")) : "");
                }
              }
            })(v.c, function(name, a) {
              v.add(decodeURIComponent(name.replace(/\+/g, " ")), a);
            });
          }
        }
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function model(value) {
        var result = reduce(value);
        if (void 0 === result) {
          throw Error("Keys are undefined");
        }
        var form = new A(null, void 0);
        value = split(value);
        /** @type {number} */
        var i = 0;
        for (; i < result.length; i++) {
          var res = result[i];
          var next = value[i];
          if (each(next)) {
            submit(form, res, next);
          } else {
            form.add(res, next);
          }
        }
        return form;
      }
      /**
       * @param {!Object} obj
       * @param {string} key
       * @return {undefined}
       */
      function _(obj, key) {
        initialize(obj);
        key = fn(obj, key);
        if (hasOwnProperty(obj.a.b, key)) {
          /** @type {null} */
          obj.c = null;
          obj.b -= obj.a.get(key).length;
          if (hasOwnProperty((obj = obj.a).b, key)) {
            delete obj.b[key];
            obj.c--;
            if (obj.a.length > 2 * obj.c) {
              selectItem(obj);
            }
          }
        }
      }
      /**
       * @param {!Object} n
       * @param {boolean} c
       * @return {?}
       */
      function appendChild(n, c) {
        return initialize(n), c = fn(n, c), hasOwnProperty(n.a.b, c);
      }
      /**
       * @param {!Object} t
       * @param {string} id
       * @param {!NodeList} entry
       * @return {undefined}
       */
      function submit(t, id, entry) {
        _(t, id);
        if (0 < entry.length) {
          /** @type {null} */
          t.c = null;
          t.a.set(fn(t, id), require(entry));
          t.b += entry.length;
        }
      }
      /**
       * @param {!Object} el
       * @param {string} id
       * @return {?}
       */
      function fn(el, id) {
        return id = String(id), el.f && (id = id.toLowerCase()), id;
      }
      /**
       * @return {undefined}
       */
      function ctor() {
        /** @type {string} */
        this.a = "";
        this.b = num;
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function is(obj) {
        return obj instanceof ctor && obj.constructor === ctor && obj.b === num ? obj.a : (sort("expected object of type Const, got '" + obj + "'"), "type_error:Const");
      }
      /**
       * @param {string} value
       * @return {?}
       */
      function make(value) {
        var result = new ctor;
        return result.a = value, result;
      }
      /**
       * @return {undefined}
       */
      function Type() {
        /** @type {string} */
        this.a = "";
        this.b = arg;
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function clean(obj) {
        return obj instanceof Type && obj.constructor === Type && obj.b === arg ? obj.a : (sort("expected object of type TrustedResourceUrl, got '" + obj + "' of type " + typeOf(obj)), "type_error:TrustedResourceUrl");
      }
      /**
       * @param {?} data
       * @param {!Object} rows
       * @return {?}
       */
      function check(data, rows) {
        var text = is(data);
        if (!resource.test(text)) {
          throw Error("Invalid TrustedResourceUrl format: " + text);
        }
        return function(a) {
          var t = new Type;
          return t.a = a, t;
        }(data = text.replace(splitter, function(source, i) {
          if (!Object.prototype.hasOwnProperty.call(rows, i)) {
            throw Error('Found marker, "' + i + '", in format string, "' + text + '", but no valid label mapping found in args: ' + JSON.stringify(rows));
          }
          return (source = rows[i]) instanceof ctor ? is(source) : encodeURIComponent(String(source));
        }));
      }
      /**
       * @return {undefined}
       */
      function type() {
        /** @type {string} */
        this.a = "";
        this.b = max;
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function normalize(value) {
        return value instanceof type && value.constructor === type && value.b === max ? value.a : (sort("expected object of type SafeUrl, got '" + value + "' of type " + typeOf(value)), "type_error:SafeUrl");
      }
      /**
       * @param {string} data
       * @return {?}
       */
      function multiply(data) {
        return data instanceof type ? data : (data = data.la ? data.ja() : String(data), JSON_START.test(data) || (data = "about:invalid#zClosurez"), fill(data));
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function fill(value) {
        var x = new type;
        return x.a = value, x;
      }
      /**
       * @return {undefined}
       */
      function Obj() {
        /** @type {string} */
        this.a = "";
        this.b = b;
      }
      /**
       * @param {string} value
       * @return {?}
       */
      function getProp(value) {
        var result = new Obj;
        return result.a = value, result;
      }
      /**
       * @param {!Object} a
       * @return {?}
       */
      function removeEventListener(a) {
        /** @type {!HTMLDocument} */
        var doc = document;
        return isString(a) ? doc.getElementById(a) : a;
      }
      /**
       * @param {!Element} element
       * @param {!Object} name
       * @return {undefined}
       */
      function load(element, name) {
        attr(name, function(value, key) {
          if (value && value.la) {
            value = value.ja();
          }
          if ("style" == key) {
            /** @type {string} */
            element.style.cssText = value;
          } else {
            if ("class" == key) {
              /** @type {string} */
              element.className = value;
            } else {
              if ("for" == key) {
                /** @type {string} */
                element.htmlFor = value;
              } else {
                if (params.hasOwnProperty(key)) {
                  element.setAttribute(params[key], value);
                } else {
                  if (0 == key.lastIndexOf("aria-", 0) || 0 == key.lastIndexOf("data-", 0)) {
                    element.setAttribute(key, value);
                  } else {
                    /** @type {string} */
                    element[key] = value;
                  }
                }
              }
            }
          }
        });
      }
      /**
       * @param {string} name
       * @param {?} element
       * @param {?} sourceVar
       * @return {?}
       */
      function createElement(name, element, sourceVar) {
        /** @type {!Arguments} */
        var args = arguments;
        /** @type {!HTMLDocument} */
        var d = document;
        /** @type {string} */
        var value = String(args[0]);
        var name = args[1];
        if (!is_defun && name && (name.name || name.type)) {
          if (value = ["<", value], name.name && value.push(' name="', createNode(name.name), '"'), name.type) {
            value.push(' type="', createNode(name.type), '"');
            var u = {};
            log(u, name);
            delete u.type;
            name = u;
          }
          value.push(">");
          /** @type {string} */
          value = value.join("");
        }
        return value = d.createElement(value), name && (isString(name) ? value.className = name : each(name) ? value.className = name.join(" ") : load(value, name)), 2 < args.length && function(s, o, values) {
          /**
           * @param {undefined} x
           * @return {undefined}
           */
          function format(x) {
            if (x) {
              o.appendChild(isString(x) ? s.createTextNode(x) : x);
            }
          }
          /** @type {number} */
          var i = 2;
          for (; i < values.length; i++) {
            var str = values[i];
            if (!isArray(str) || isFunction(str) && 0 < str.nodeType) {
              format(str);
            } else {
              equal(endsWith(str) ? require(str) : str, format);
            }
          }
        }(d, value, args), value;
      }
      /**
       * @param {!Object} val
       * @return {?}
       */
      function endsWith(val) {
        if (val && "number" == typeof val.length) {
          if (isFunction(val)) {
            return "function" == typeof val.item || "string" == typeof val.item;
          }
          if (isNaN(val)) {
            return "function" == typeof val.item;
          }
        }
        return false;
      }
      /**
       * @param {boolean} name
       * @return {?}
       */
      function addItem(name) {
        /** @type {!Array} */
        var row2 = [];
        return function callback(e, value, out) {
          if (null == value) {
            out.push("null");
          } else {
            if ("object" == typeof value) {
              if (each(value)) {
                /** @type {string} */
                var m = value;
                value = m.length;
                out.push("[");
                /** @type {string} */
                var o = "";
                /** @type {number} */
                var item = 0;
                for (; item < value; item++) {
                  out.push(o);
                  callback(e, m[item], out);
                  /** @type {string} */
                  o = ",";
                }
                return void out.push("]");
              }
              if (!(value instanceof String || value instanceof Number || value instanceof Boolean)) {
                for (m in out.push("{"), o = "", value) {
                  if (Object.prototype.hasOwnProperty.call(value, m)) {
                    if ("function" != typeof(item = value[m])) {
                      out.push(o);
                      compile(m, out);
                      out.push(":");
                      callback(e, item, out);
                      /** @type {string} */
                      o = ",";
                    }
                  }
                }
                return void out.push("}");
              }
              /** @type {*} */
              value = value.valueOf();
            }
            switch(typeof value) {
              case "string":
                compile(value, out);
                break;
              case "number":
                out.push(isFinite(value) && !isNaN(value) ? String(value) : "null");
                break;
              case "boolean":
                out.push(String(value));
                break;
              case "function":
                out.push("null");
                break;
              default:
                throw Error("Unknown type: " + typeof value);
            }
          }
        }(new function() {
        }, name, row2), row2.join("");
      }
      /**
       * @param {string} className
       * @param {!Array} str
       * @return {undefined}
       */
      function compile(className, str) {
        str.push('"', className.replace(rx, function(id) {
          var e = traced[id];
          return e || (e = "\\u" + (65536 | id.charCodeAt(0)).toString(16).substr(1), traced[id] = e), e;
        }), '"');
      }
      /**
       * @return {?}
       */
      function retry() {
        var result = end();
        return window && !!element && 11 == element || /Edge\/\d+/.test(result);
      }
      /**
       * @return {?}
       */
      function updatePresenterWindow() {
        return global.window && global.window.location.href || self && self.location && self.location.href || "";
      }
      /**
       * @param {string} value
       * @param {string} context
       * @return {undefined}
       */
      function finish(value, context) {
        context = context || global.window;
        /** @type {string} */
        var x = "about:blank";
        if (value) {
          x = normalize(multiply(value));
        }
        context.location.href = x;
      }
      /**
       * @param {number} name
       * @return {?}
       */
      function isMobile(name) {
        return !!((name = (name || end()).toLowerCase()).match(/android/) || name.match(/webos/) || name.match(/iphone|ipad|ipod/) || name.match(/blackberry/) || name.match(/windows phone/) || name.match(/iemobile/));
      }
      /**
       * @param {!Object} context
       * @return {undefined}
       */
      function isObject(context) {
        context = context || global.window;
        try {
          context.close();
        } catch (t) {
        }
      }
      /**
       * @param {(Object|string)} obj
       * @param {!Object} data
       * @param {string} target
       * @return {?}
       */
      function render(obj, data, target) {
        /** @type {string} */
        var self = Math.floor(1E9 * Math.random()).toString();
        data = data || 500;
        target = target || 600;
        /** @type {number} */
        var top = (window.screen.availHeight - target) / 2;
        /** @type {number} */
        var left = (window.screen.availWidth - data) / 2;
        for (a in data = {
          width : data,
          height : target,
          top : 0 < top ? top : 0,
          left : 0 < left ? left : 0,
          location : true,
          resizable : true,
          statusbar : true,
          toolbar : false
        }, target = end().toLowerCase(), self && (data.target = self, has(target, "crios/") && (data.target = "_blank")), detect(end()) == ios && (obj = obj || "http://localhost", data.scrollbars = true), target = obj || "", (obj = data) || (obj = {}), self = window, data = target instanceof type ? target : multiply(void 0 !== target.href ? target.href : String(target)), target = obj.target || target.target, top = [], obj) {
          switch(a) {
            case "width":
            case "height":
            case "top":
            case "left":
              top.push(a + "=" + obj[a]);
              break;
            case "target":
            case "noopener":
            case "noreferrer":
              break;
            default:
              top.push(a + "=" + (obj[a] ? 1 : 0));
          }
        }
        /** @type {string} */
        var a = top.join(",");
        if ((floor("iPhone") && !floor("iPod") && !floor("iPad") || floor("iPad") || floor("iPod")) && self.navigator && self.navigator.standalone && target && "_self" != target ? (a = self.document.createElement("A"), data instanceof type || data instanceof type || (data = data.la ? data.ja() : String(data), JSON_START.test(data) || (data = "about:invalid#zClosurez"), data = fill(data)), a.href = normalize(data), a.setAttribute("target", target), obj.noreferrer && a.setAttribute("rel", "noreferrer"), 
        (obj = document.createEvent("MouseEvent")).initMouseEvent("click", true, true, self, 1), a.dispatchEvent(obj), a = {}) : obj.noreferrer ? (a = self.open("", target, a), obj = normalize(data), a && (current && has(obj, ";") && (obj = "'" + obj.replace(/'/g, "%27") + "'"), a.opener = null, make("b/12014412, meta tag with sanitized URL"), obj = getProp(obj = '<meta name="referrer" content="no-referrer"><meta http-equiv="refresh" content="0; url=' + createNode(obj) + '">'), a.document.write(function(obj) {
          return obj instanceof Obj && obj.constructor === Obj && obj.b === b ? obj.a : (sort("expected object of type SafeHtml, got '" + obj + "' of type " + typeOf(obj)), "type_error:SafeHtml");
        }(obj)), a.document.close())) : (a = self.open(normalize(data), target, a)) && obj.noopener && (a.opener = null), a) {
          try {
            a.focus();
          } catch (t) {
          }
        }
        return a;
      }
      /**
       * @return {?}
       */
      function play() {
        /** @type {null} */
        var callback = null;
        return (new Promise(function(createElement) {
          if ("complete" == global.document.readyState) {
            createElement();
          } else {
            /**
             * @return {undefined}
             */
            callback = function() {
              createElement();
            };
            send(window, "load", callback);
          }
        })).m(function(boardManager) {
          throw add(window, "load", callback), boardManager;
        });
      }
      /**
       * @param {string} quality
       * @return {?}
       */
      function resize(quality) {
        return quality = quality || end(), !("file:" !== warn() || !quality.toLowerCase().match(/iphone|ipad|ipod|android/));
      }
      /**
       * @return {?}
       */
      function click() {
        var win = global.window;
        try {
          return !(!win || win == win.top);
        } catch (t) {
          return false;
        }
      }
      /**
       * @return {?}
       */
      function replace() {
        return "object" != typeof global.window && "function" == typeof global.importScripts;
      }
      /**
       * @return {?}
       */
      function css() {
        return config.INTERNAL.hasOwnProperty("reactNative") ? "ReactNative" : config.INTERNAL.hasOwnProperty("node") ? "Node" : replace() ? "Worker" : "Browser";
      }
      /**
       * @return {?}
       */
      function getHeight() {
        var g = css();
        return "ReactNative" === g || "Node" === g;
      }
      /**
       * @param {string} str
       * @return {?}
       */
      function detect(str) {
        var ua = str.toLowerCase();
        return has(ua, "opera/") || has(ua, "opr/") || has(ua, "opios/") ? "Opera" : has(ua, "iemobile") ? "IEMobile" : has(ua, "msie") || has(ua, "trident/") ? "IE" : has(ua, "edge/") ? "Edge" : has(ua, "firefox/") ? ios : has(ua, "silk/") ? "Silk" : has(ua, "blackberry") ? "Blackberry" : has(ua, "webos") ? "Webos" : !has(ua, "safari/") || has(ua, "chrome/") || has(ua, "crios/") || has(ua, "android") ? !has(ua, "chrome/") && !has(ua, "crios/") || has(ua, "edge/") ? has(ua, "android") ? "Android" : 
        (str = str.match(/([a-zA-Z\d\.]+)\/[a-zA-Z\d\.]*$/)) && 2 == str.length ? str[1] : "Other" : months : "Safari";
      }
      /**
       * @param {string} c
       * @param {!Object} x
       * @return {?}
       */
      function success(c, x) {
        x = x || [];
        var i;
        /** @type {!Array} */
        var ret = [];
        var match = {};
        for (i in names) {
          /** @type {boolean} */
          match[names[i]] = true;
        }
        /** @type {number} */
        i = 0;
        for (; i < x.length; i++) {
          if (void 0 !== match[x[i]]) {
            delete match[x[i]];
            ret.push(x[i]);
          }
        }
        return ret.sort(), (x = ret).length || (x = ["FirebaseCore-web"]), "Browser" === (ret = css()) ? ret = detect(match = end()) : "Worker" === ret && (ret = detect(match = end()) + "-" + ret), ret + "/JsCore/" + c + "/" + x.join(",");
      }
      /**
       * @return {?}
       */
      function end() {
        return global.navigator && global.navigator.userAgent || "";
      }
      /**
       * @param {string} props
       * @param {string} object
       * @return {?}
       */
      function stringify(props, object) {
        props = props.split(".");
        object = object || global;
        /** @type {number} */
        var i = 0;
        for (; i < props.length && "object" == typeof object && null != object; i++) {
          object = object[props[i]];
        }
        return i != props.length && (object = void 0), object;
      }
      /**
       * @return {?}
       */
      function detectPrivateMode() {
        try {
          var storage = global.localStorage;
          var i = removeItem();
          if (storage) {
            return storage.setItem(i, "1"), storage.removeItem(i), !retry() || !!global.indexedDB;
          }
        } catch (t) {
          return replace() && !!global.indexedDB;
        }
        return false;
      }
      /**
       * @return {?}
       */
      function size() {
        return (migrateWarn() || "chrome-extension:" === warn() || resize()) && !getHeight() && detectPrivateMode() && !replace();
      }
      /**
       * @return {?}
       */
      function migrateWarn() {
        return "http:" === warn() || "https:" === warn();
      }
      /**
       * @return {?}
       */
      function warn() {
        return global.location && global.location.protocol || null;
      }
      /**
       * @param {string} data
       * @return {?}
       */
      function query(data) {
        return !isMobile(data = data || end()) && detect(data) != ios;
      }
      /**
       * @param {?} value
       * @return {?}
       */
      function parse(value) {
        return void 0 === value ? null : addItem(value);
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function $getDRtoDL(obj) {
        var name;
        var macroDict = {};
        for (name in obj) {
          if (obj.hasOwnProperty(name) && null !== obj[name] && void 0 !== obj[name]) {
            macroDict[name] = obj[name];
          }
        }
        return macroDict;
      }
      /**
       * @param {number} body
       * @return {?}
       */
      function dispatch(body) {
        if (null !== body) {
          return JSON.parse(body);
        }
      }
      /**
       * @param {string} url
       * @return {?}
       */
      function removeItem(url) {
        return url || Math.floor(1E9 * Math.random()).toString();
      }
      /**
       * @param {string} letter
       * @return {?}
       */
      function process(letter) {
        return "Safari" != detect(letter = letter || end()) && !letter.toLowerCase().match(/iphone|ipad|ipod/);
      }
      /**
       * @return {undefined}
       */
      function keydown() {
        var state = global.___jsl;
        if (state && state.H) {
          var i;
          for (i in state.H) {
            if (state.H[i].r = state.H[i].r || [], state.H[i].L = state.H[i].L || [], state.H[i].r = state.H[i].L.concat(), state.CP) {
              /** @type {number} */
              var wcIndex = 0;
              for (; wcIndex < state.CP.length; wcIndex++) {
                /** @type {null} */
                state.CP[wcIndex] = null;
              }
            }
          }
        }
      }
      /**
       * @param {(number|string)} value
       * @param {string} data
       * @return {undefined}
       */
      function link(value, data) {
        if (value > data) {
          throw Error("Short delay should be less than long delay!");
        }
        /** @type {(number|string)} */
        this.a = value;
        /** @type {string} */
        this.c = data;
        value = end();
        data = css();
        this.b = isMobile(value) || "ReactNative" === data;
      }
      /**
       * @return {?}
       */
      function setup() {
        var document = global.document;
        return !document || void 0 === document.visibilityState || "visible" == document.visibilityState;
      }
      /**
       * @param {number} date
       * @return {?}
       */
      function formatDate(date) {
        try {
          /** @type {!Date} */
          var now = new Date(parseInt(date, 10));
          if (!isNaN(now.getTime()) && !/[^0-9]/.test(date)) {
            return now.toUTCString();
          }
        } catch (t) {
        }
        return null;
      }
      /**
       * @return {?}
       */
      function applyChange() {
        return !(!stringify("fireauth.oauthhelper", global) && !stringify("fireauth.iframe", global));
      }
      /**
       * @param {!Function} obj
       * @param {string} key
       * @param {string} type
       * @return {undefined}
       */
      function debug(obj, key, type) {
        if (Ti) {
          Object.defineProperty(obj, key, {
            configurable : true,
            enumerable : true,
            value : type
          });
        } else {
          /** @type {string} */
          obj[key] = type;
        }
      }
      /**
       * @param {!Function} arg
       * @param {!Object} data
       * @return {undefined}
       */
      function done(arg, data) {
        if (data) {
          var i;
          for (i in data) {
            if (data.hasOwnProperty(i)) {
              debug(arg, i, data[i]);
            }
          }
        }
      }
      /**
       * @param {!Object} v
       * @return {?}
       */
      function err(v) {
        var result = {};
        return done(result, v), result;
      }
      /**
       * @param {?} obj
       * @return {?}
       */
      function _get(obj) {
        var result = obj;
        if ("object" == typeof obj && null != obj) {
          var key;
          for (key in result = "length" in obj ? [] : {}, obj) {
            debug(result, key, _get(obj[key]));
          }
        }
        return result;
      }
      /**
       * @param {string} name
       * @param {string} message
       * @return {undefined}
       */
      function Buffer(name, message) {
        /** @type {string} */
        this.code = code + name;
        this.message = message || status[name] || "";
      }
      /**
       * @param {!Object} result
       * @return {?}
       */
      function getData(result) {
        var value = result && result.code;
        return value ? new Buffer(value.substring(code.length), result.message) : null;
      }
      /**
       * @param {string} result
       * @return {undefined}
       */
      function Query(result) {
        var val = result[uri];
        if (void 0 === val) {
          throw new Buffer("missing-continue-uri");
        }
        if ("string" != typeof val || "string" == typeof val && !val.length) {
          throw new Buffer("invalid-continue-uri");
        }
        /** @type {string} */
        this.h = val;
        /** @type {null} */
        this.b = this.a = null;
        /** @type {boolean} */
        this.g = false;
        var value = result[k];
        if (value && "object" == typeof value) {
          val = value[what];
          var result = value[token];
          if (value = value[metricName], "string" == typeof val && val.length) {
            if (this.a = val, void 0 !== result && "boolean" != typeof result) {
              throw new Buffer("argument-error", token + " property must be a boolean when specified.");
            }
            if (this.g = !!result, void 0 !== value && ("string" != typeof value || "string" == typeof value && !value.length)) {
              throw new Buffer("argument-error", metricName + " property must be a non empty string when specified.");
            }
            this.b = value || null;
          } else {
            if (void 0 !== val) {
              throw new Buffer("argument-error", what + " property must be a non empty string when specified.");
            }
            if (void 0 !== result || void 0 !== value) {
              throw new Buffer("missing-android-pkg-name");
            }
          }
        } else {
          if (void 0 !== value) {
            throw new Buffer("argument-error", k + " property must be a non null object when specified.");
          }
        }
        if (this.f = null, (val = result[step]) && "object" == typeof val) {
          if ("string" == typeof(val = val[operator]) && val.length) {
            this.f = val;
          } else {
            if (void 0 !== val) {
              throw new Buffer("argument-error", operator + " property must be a non empty string when specified.");
            }
          }
        } else {
          if (void 0 !== val) {
            throw new Buffer("argument-error", step + " property must be a non null object when specified.");
          }
        }
        if (void 0 !== (result = result[aKey]) && "boolean" != typeof result) {
          throw new Buffer("argument-error", aKey + " property must be a boolean when specified.");
        }
        /** @type {boolean} */
        this.c = !!result;
      }
      /**
       * @param {!Object} c
       * @return {?}
       */
      function getComputedColor(c) {
        var color = {};
        var dimension;
        for (dimension in color.continueUrl = c.h, color.canHandleCodeInApp = c.c, (color.androidPackageName = c.a) && (color.androidMinimumVersion = c.b, color.androidInstallApp = c.g), color.iOSBundleId = c.f, color) {
          if (null === color[dimension]) {
            delete color[dimension];
          }
        }
        return color;
      }
      /**
       * @param {string} value
       * @return {?}
       */
      function transform(value) {
        /** @type {string} */
        var result = "";
        return function(text, f) {
          /**
           * @param {number} s
           * @return {?}
           */
          function n(s) {
            for (; i < text.length;) {
              var name = text.charAt(i++);
              var s = o[name];
              if (null != s) {
                return s;
              }
              if (!/^[\s\xa0]*$/.test(name)) {
                throw Error("Unknown base64 encoding at char: " + name);
              }
            }
            return s;
          }
          !function() {
            if (!hash) {
              hash = {};
              o = {};
              /** @type {number} */
              var p = 0;
              for (; 65 > p; p++) {
                /** @type {string} */
                hash[p] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=".charAt(p);
                /** @type {number} */
                o[hash[p]] = p;
                if (62 <= p) {
                  /** @type {number} */
                  o["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.".charAt(p)] = p;
                }
              }
            }
          }();
          /** @type {number} */
          var i = 0;
          for (;;) {
            var s = n(-1);
            var o = n(0);
            var end = n(64);
            var c = n(64);
            if (64 === c && -1 === s) {
              break;
            }
            f(s << 2 | o >> 4);
            if (64 != end) {
              f(o << 4 & 240 | end >> 2);
              if (64 != c) {
                f(end << 6 & 192 | c);
              }
            }
          }
        }(value, function(intermediate) {
          result = result + String.fromCharCode(intermediate);
        }), result;
      }
      /**
       * @param {!Object} a
       * @return {undefined}
       */
      function Foo(a) {
        this.c = a.sub;
        this.a = a.provider_id || a.firebase && a.firebase.sign_in_provider || null;
        /** @type {boolean} */
        this.b = !!a.is_anonymous || "anonymous" == this.a;
      }
      /**
       * @param {!Object} e
       * @return {?}
       */
      function encode(e) {
        return (e = ev(e)) && e.sub && e.iss && e.aud && e.exp ? new Foo(e) : null;
      }
      /**
       * @param {string} type
       * @return {?}
       */
      function ev(type) {
        if (!type) {
          return null;
        }
        if (3 != (type = type.split(".")).length) {
          return null;
        }
        /** @type {number} */
        var bt = (4 - (type = type[1]).length % 4) % 4;
        /** @type {number} */
        var at = 0;
        for (; at < bt; at++) {
          /** @type {string} */
          type = type + ".";
        }
        try {
          return JSON.parse(transform(type));
        } catch (t) {
        }
        return null;
      }
      /**
       * @param {string} displayText
       * @return {?}
       */
      function info(displayText) {
        var bookIdIndex;
        for (bookIdIndex in bookIDs) {
          if (bookIDs[bookIdIndex].Na == displayText) {
            return bookIDs[bookIdIndex];
          }
        }
        return null;
      }
      /**
       * @param {string} obj
       * @return {?}
       */
      function pick(obj) {
        var tags = {};
        /** @type {function(?): undefined} */
        tags["facebook.com"] = parseError;
        /** @type {function(?): undefined} */
        tags["google.com"] = successCb;
        /** @type {function(?): undefined} */
        tags["github.com"] = Provider;
        /** @type {function(!Object): undefined} */
        tags["twitter.com"] = tag;
        var name = obj && obj[property];
        try {
          if (name) {
            return tags[name] ? new tags[name](obj) : new settings(obj);
          }
          if (void 0 !== obj[j]) {
            return new describe(obj);
          }
        } catch (t) {
        }
        return null;
      }
      /**
       * @param {!Object} item
       * @return {undefined}
       */
      function describe(item) {
        var type = item[property];
        if (!type && item[j]) {
          var value = encode(item[j]);
          if (value && value.a) {
            type = value.a;
          }
        }
        if (!type) {
          throw Error("Invalid additional user info!");
        }
        if (!("anonymous" != type && "custom" != type)) {
          /** @type {null} */
          type = null;
        }
        /** @type {boolean} */
        value = false;
        if (void 0 !== item.isNewUser) {
          /** @type {boolean} */
          value = !!item.isNewUser;
        } else {
          if ("identitytoolkit#SignupNewUserResponse" === item.kind) {
            /** @type {boolean} */
            value = true;
          }
        }
        debug(this, "providerId", type);
        debug(this, "isNewUser", value);
      }
      /**
       * @param {?} callback
       * @return {undefined}
       */
      function settings(callback) {
        describe.call(this, callback);
        debug(this, "profile", _get((callback = dispatch(callback.rawUserInfo || "{}")) || {}));
      }
      /**
       * @param {?} file
       * @return {undefined}
       */
      function parseError(file) {
        if (settings.call(this, file), "facebook.com" != this.providerId) {
          throw Error("Invalid provider ID!");
        }
      }
      /**
       * @param {?} props
       * @return {undefined}
       */
      function Provider(props) {
        if (settings.call(this, props), "github.com" != this.providerId) {
          throw Error("Invalid provider ID!");
        }
        debug(this, "username", this.profile && this.profile.login || null);
      }
      /**
       * @param {?} data
       * @return {undefined}
       */
      function successCb(data) {
        if (settings.call(this, data), "google.com" != this.providerId) {
          throw Error("Invalid provider ID!");
        }
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function tag(data) {
        if (settings.call(this, data), "twitter.com" != this.providerId) {
          throw Error("Invalid provider ID!");
        }
        debug(this, "username", data.screenName || null);
      }
      /**
       * @param {string} value
       * @return {?}
       */
      function slice(value) {
        var result = getValue(value);
        var key = isFinite(result, "link");
        var bundlerPath = isFinite(getValue(key), "link");
        return isFinite(getValue(result = isFinite(result, "deep_link_id")), "link") || result || bundlerPath || key || value;
      }
      /**
       * @param {?} q
       * @param {string} n
       * @return {?}
       */
      function t(q, n) {
        return q.then(function(attribs) {
          if (attribs[i]) {
            var item = encode(attribs[i]);
            if (!item || n != item.c) {
              throw new Buffer("user-mismatch");
            }
            return attribs;
          }
          throw new Buffer("user-mismatch");
        }).m(function(_exit) {
          throw _exit && _exit.code && _exit.code == code + "user-not-found" ? new Buffer("user-mismatch") : _exit;
        });
      }
      /**
       * @param {string} val
       * @param {!Object} options
       * @param {string} args
       * @return {undefined}
       */
      function options(val, options, args) {
        if (options.idToken || options.accessToken) {
          if (options.idToken) {
            debug(this, "idToken", options.idToken);
          }
          if (options.accessToken) {
            debug(this, "accessToken", options.accessToken);
          }
        } else {
          if (!options.oauthToken || !options.oauthTokenSecret) {
            throw new Buffer("internal-error", "failed to construct a credential");
          }
          debug(this, "accessToken", options.oauthToken);
          debug(this, "secret", options.oauthTokenSecret);
        }
        debug(this, "providerId", val);
        debug(this, "signInMethod", args);
      }
      /**
       * @param {!Object} response
       * @return {?}
       */
      function doRequest(response) {
        var obj = {};
        return response.idToken && (obj.id_token = response.idToken), response.accessToken && (obj.access_token = response.accessToken), response.secret && (obj.oauth_token_secret = response.secret), obj.providerId = response.providerId, {
          postBody : model(obj).toString(),
          requestUri : "http://localhost"
        };
      }
      /**
       * @param {string} pCommand
       * @param {!Array} pEvent
       * @return {undefined}
       */
      function g(pCommand, pEvent) {
        this.Ac = pEvent || [];
        done(this, {
          providerId : pCommand,
          isOAuthProvider : true
        });
        this.tb = {};
        this.Za = (info(pCommand) || {}).Ma || null;
        /** @type {null} */
        this.Xa = null;
      }
      /**
       * @param {?} a
       * @return {undefined}
       */
      function self(a) {
        g.call(this, a, props);
        /** @type {!Array} */
        this.a = [];
      }
      /**
       * @return {undefined}
       */
      function cache() {
        self.call(this, "facebook.com");
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function then(value) {
        if (!value) {
          throw new Buffer("argument-error", "credential failed: expected 1 argument (the OAuth access token).");
        }
        /** @type {!Object} */
        var m = value;
        return isFunction(value) && (m = value.accessToken), (new cache).credential(null, m);
      }
      /**
       * @return {undefined}
       */
      function array() {
        self.call(this, "github.com");
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function extract(value) {
        if (!value) {
          throw new Buffer("argument-error", "credential failed: expected 1 argument (the OAuth access token).");
        }
        /** @type {!Object} */
        var m = value;
        return isFunction(value) && (m = value.accessToken), (new array).credential(null, m);
      }
      /**
       * @return {undefined}
       */
      function def() {
        self.call(this, "google.com");
        this.ta("profile");
      }
      /**
       * @param {!Object} result
       * @param {string} password
       * @return {?}
       */
      function isValid(result, password) {
        /** @type {!Object} */
        var email = result;
        return isFunction(result) && (email = result.idToken, password = result.accessToken), (new def).credential(email, password);
      }
      /**
       * @return {undefined}
       */
      function template() {
        g.call(this, "twitter.com", onResponse);
      }
      /**
       * @param {!Object} val
       * @param {?} from
       * @return {?}
       */
      function inArray(val, from) {
        /** @type {!Object} */
        var data = val;
        if (isFunction(data) || (data = {
          oauthToken : val,
          oauthTokenSecret : from
        }), !data.oauthToken || !data.oauthTokenSecret) {
          throw new Buffer("argument-error", "credential failed: expected 2 arguments (the OAuth access token and secret).");
        }
        return new options("twitter.com", data, "twitter.com");
      }
      /**
       * @param {number} _
       * @param {number} s
       * @param {string} n
       * @return {undefined}
       */
      function Template(_, s, n) {
        /** @type {number} */
        this.a = _;
        /** @type {number} */
        this.b = s;
        debug(this, "providerId", "password");
        debug(this, "signInMethod", n === item.EMAIL_LINK_SIGN_IN_METHOD ? item.EMAIL_LINK_SIGN_IN_METHOD : item.EMAIL_PASSWORD_SIGN_IN_METHOD);
      }
      /**
       * @return {undefined}
       */
      function item() {
        done(this, {
          providerId : "password",
          isOAuthProvider : false
        });
      }
      /**
       * @param {string} parent
       * @param {string} e
       * @return {?}
       */
      function cleanup(parent, e) {
        if (!(e = walk(e))) {
          throw new Buffer("argument-error", "Invalid email link!");
        }
        return new Template(parent, e, item.EMAIL_LINK_SIGN_IN_METHOD);
      }
      /**
       * @param {string} b
       * @return {?}
       */
      function walk(b) {
        var e = isFinite((b = new function(value) {
          this.a = getValue(value);
        }(b = slice(b))).a, "oobCode") || null;
        return "signIn" === (isFinite(b.a, "mode") || null) && e ? e : null;
      }
      /**
       * @param {number} value
       * @return {undefined}
       */
      function source(value) {
        if (!(value.Ra && value.Qa || value.Ea && value.Z)) {
          throw new Buffer("internal-error");
        }
        /** @type {number} */
        this.a = value;
        debug(this, "providerId", "phone");
        debug(this, "signInMethod", "phone");
      }
      /**
       * @param {(number|string)} input
       * @return {?}
       */
      function Transform(input) {
        return input.a.Ea && input.a.Z ? {
          temporaryProof : input.a.Ea,
          phoneNumber : input.a.Z
        } : {
          sessionInfo : input.a.Ra,
          code : input.a.Qa
        };
      }
      /**
       * @param {string} a
       * @return {undefined}
       */
      function d(a) {
        try {
          this.a = a || config.auth();
        } catch (t) {
          throw new Buffer("argument-error", "Either an instance of firebase.auth.Auth must be passed as an argument to the firebase.auth.PhoneAuthProvider constructor, or the default firebase App instance must be initialized via firebase.initializeApp().");
        }
        done(this, {
          providerId : "phone",
          isOAuthProvider : false
        });
      }
      /**
       * @param {?} url
       * @param {!Object} options
       * @return {?}
       */
      function navigate(url, options) {
        if (!url) {
          throw new Buffer("missing-verification-id");
        }
        if (!options) {
          throw new Buffer("missing-verification-code");
        }
        return new source({
          Ra : url,
          Qa : options
        });
      }
      /**
       * @param {!Object} data
       * @return {?}
       */
      function indexOf(data) {
        if (data.temporaryProof && data.phoneNumber) {
          return new source({
            Ea : data.temporaryProof,
            Z : data.phoneNumber
          });
        }
        var name = data && data.providerId;
        if (!name || "password" === name) {
          return null;
        }
        var value = data && data.oauthAccessToken;
        var start = data && data.oauthTokenSecret;
        data = data && data.oauthIdToken;
        try {
          switch(name) {
            case "google.com":
              return isValid(data, value);
            case "facebook.com":
              return then(value);
            case "github.com":
              return extract(value);
            case "twitter.com":
              return inArray(value, start);
            default:
              return (new self(name)).credential(data, value);
          }
        } catch (t) {
          return null;
        }
      }
      /**
       * @param {!Object} type
       * @return {undefined}
       */
      function rewrite(type) {
        if (!type.isOAuthProvider) {
          throw new Buffer("invalid-oauth-provider");
        }
      }
      /**
       * @param {number} b
       * @param {string} c
       * @param {string} f
       * @param {string} g
       * @param {string} a
       * @return {undefined}
       */
      function Node(b, c, f, g, a) {
        if (this.b = b, this.c = c || null, this.f = f || null, this.g = g || null, this.a = a || null, !this.f && !this.a) {
          throw new Buffer("invalid-auth-event");
        }
        if (this.f && this.a) {
          throw new Buffer("invalid-auth-event");
        }
        if (this.f && !this.g) {
          throw new Buffer("invalid-auth-event");
        }
      }
      /**
       * @param {!Object} node
       * @return {?}
       */
      function stop(node) {
        return (node = node || {}).type ? new Node(node.type, node.eventId, node.urlResponse, node.sessionId, node.error && getData(node.error)) : null;
      }
      /**
       * @return {undefined}
       */
      function tester() {
        /** @type {null} */
        this.b = null;
        /** @type {!Array} */
        this.a = [];
      }
      /**
       * @param {string} b
       * @return {undefined}
       */
      function ui(b) {
        /** @type {string} */
        var data = "unauthorized-domain";
        var length = void 0;
        var a = getValue(b);
        b = a.b;
        if ("chrome-extension" == (a = a.c)) {
          length = extend("This chrome extension ID (chrome-extension://%s) is not authorized to run this operation. Add it to the OAuth redirect domains list in the Firebase console -> Auth section -> Sign in method tab.", b);
        } else {
          if ("http" == a || "https" == a) {
            length = extend("This domain (%s) is not authorized to run this operation. Add it to the OAuth redirect domains list in the Firebase console -> Auth section -> Sign in method tab.", b);
          } else {
            /** @type {string} */
            data = "operation-not-supported-in-this-environment";
          }
        }
        Buffer.call(this, data, length);
      }
      /**
       * @param {!Object} data
       * @param {string} val
       * @param {?} buffer
       * @return {undefined}
       */
      function Comment(data, val, buffer) {
        Buffer.call(this, data, buffer);
        if ((data = val || {}).ub) {
          debug(this, "email", data.ub);
        }
        if (data.Z) {
          debug(this, "phoneNumber", data.Z);
        }
        if (data.credential) {
          debug(this, "credential", data.credential);
        }
      }
      /**
       * @param {!Object} data
       * @return {?}
       */
      function serialize(data) {
        if (data.code) {
          var content = data.code || "";
          if (0 == content.indexOf(code)) {
            content = content.substring(code.length);
          }
          var p = {
            credential : indexOf(data)
          };
          if (data.email) {
            p.ub = data.email;
          } else {
            if (!data.phoneNumber) {
              return new Buffer(content, data.message || void 0);
            }
            p.Z = data.phoneNumber;
          }
          return new Comment(content, p, data.message);
        }
        return null;
      }
      /**
       * @return {undefined}
       */
      function src() {
      }
      /**
       * @param {!Object} record
       * @return {?}
       */
      function convert(record) {
        return record.c || (record.c = record.b());
      }
      /**
       * @return {undefined}
       */
      function testObject() {
      }
      /**
       * @param {?} item
       * @return {?}
       */
      function inspect(item) {
        if (!item.f && "undefined" == typeof XMLHttpRequest && "undefined" != typeof ActiveXObject) {
          /** @type {!Array} */
          var bits = ["MSXML2.XMLHTTP.6.0", "MSXML2.XMLHTTP.3.0", "MSXML2.XMLHTTP", "Microsoft.XMLHTTP"];
          /** @type {number} */
          var n = 0;
          for (; n < bits.length; n++) {
            var i = bits[n];
            try {
              return new ActiveXObject(i), item.f = i;
            } catch (t) {
            }
          }
          throw Error("Could not create ActiveXObject. ActiveX might be disabled, or MSXML might not be installed");
        }
        return item.f;
      }
      /**
       * @return {undefined}
       */
      function cc() {
      }
      /**
       * @return {undefined}
       */
      function xhr() {
        /** @type {!XDomainRequest} */
        this.a = new XDomainRequest;
        /** @type {number} */
        this.readyState = 0;
        /** @type {null} */
        this.onreadystatechange = null;
        /** @type {string} */
        this.responseText = "";
        /** @type {number} */
        this.status = -1;
        /** @type {string} */
        this.statusText = "";
        this.a.onload = bind(this.bc, this);
        this.a.onerror = bind(this.zb, this);
        this.a.onprogress = bind(this.cc, this);
        this.a.ontimeout = bind(this.fc, this);
      }
      /**
       * @param {!XMLHttpRequest} request
       * @param {string} state
       * @return {undefined}
       */
      function readyStateChange(request, state) {
        /** @type {string} */
        request.readyState = state;
        if (request.onreadystatechange) {
          request.onreadystatechange();
        }
      }
      /**
       * @param {?} value
       * @param {?} options
       * @param {?} callback
       * @return {undefined}
       */
      function Parser(value, options, callback) {
        this.reset(value, options, callback, void 0, void 0);
      }
      /**
       * @param {!Function} options
       * @return {undefined}
       */
      function Constraint(options) {
        /** @type {!Function} */
        this.f = options;
        /** @type {null} */
        this.b = this.c = this.a = null;
      }
      /**
       * @param {string} _at_name
       * @param {!Object} _at_value
       * @return {undefined}
       */
      function Param(_at_name, _at_value) {
        /** @type {string} */
        this.name = _at_name;
        /** @type {!Object} */
        this.value = _at_value;
      }
      /**
       * @param {string} name
       * @return {?}
       */
      function parser(name) {
        var data;
        if (c || (c = new Constraint(""), buffer[""] = c, c.c = parent), !(data = buffer[name])) {
          data = new Constraint(name);
          var n = name.lastIndexOf(".");
          var i = name.substr(n + 1);
          if (!(n = parser(name.substr(0, n))).b) {
            n.b = {};
          }
          n.b[i] = data;
          data.a = n;
          buffer[name] = data;
        }
        return data;
      }
      /**
       * @param {!Object} app
       * @param {string} url
       * @return {undefined}
       */
      function runner(app, url) {
        if (app) {
          app.log(param, url, void 0);
        }
      }
      /**
       * @param {!Function} fn
       * @return {undefined}
       */
      function Worker(fn) {
        /** @type {!Function} */
        this.f = fn;
      }
      /**
       * @param {number} b
       * @return {undefined}
       */
      function f(b) {
        p.call(this);
        /** @type {number} */
        this.j = b;
        /** @type {number} */
        this.readyState = CLOSED;
        /** @type {number} */
        this.status = 0;
        /** @type {string} */
        this.responseText = this.statusText = "";
        /** @type {null} */
        this.onreadystatechange = null;
        /** @type {!Headers} */
        this.g = new Headers;
        /** @type {null} */
        this.b = null;
        /** @type {string} */
        this.h = "GET";
        /** @type {string} */
        this.c = "";
        /** @type {boolean} */
        this.a = false;
        this.f = parser("goog.net.FetchXmlHttp");
      }
      /**
       * @param {!Node} f
       * @return {undefined}
       */
      function setState(f) {
        if (f.onreadystatechange) {
          f.onreadystatechange.call(f);
        }
      }
      /**
       * @param {string} bExpanded
       * @return {undefined}
       */
      function node(bExpanded) {
        p.call(this);
        this.headers = new Vector;
        this.D = bExpanded || null;
        /** @type {boolean} */
        this.c = false;
        /** @type {null} */
        this.B = this.a = null;
        /** @type {string} */
        this.h = this.N = this.l = "";
        /** @type {boolean} */
        this.f = this.I = this.j = this.G = false;
        /** @type {number} */
        this.g = 0;
        /** @type {null} */
        this.s = null;
        /** @type {string} */
        this.o = other;
        /** @type {boolean} */
        this.v = this.O = false;
      }
      /**
       * @param {!Object} self
       * @param {!Object} data
       * @param {string} type
       * @param {string} id
       * @param {string} styles
       * @return {?}
       */
      function get(self, data, type, id, styles) {
        if (self.a) {
          throw Error("[goog.net.XhrIo] Object is active with another request=" + self.l + "; newUri=" + data);
        }
        type = type ? type.toUpperCase() : "GET";
        /** @type {!Object} */
        self.l = data;
        /** @type {string} */
        self.h = "";
        /** @type {string} */
        self.N = type;
        /** @type {boolean} */
        self.G = false;
        /** @type {boolean} */
        self.c = true;
        self.a = self.D ? self.D.a() : value.a();
        self.B = self.D ? convert(self.D) : convert(value);
        self.a.onreadystatechange = bind(self.Db, self);
        try {
          runner(self.b, right(self, "Opening Xhr"));
          /** @type {boolean} */
          self.I = true;
          self.a.open(type, String(data), true);
          /** @type {boolean} */
          self.I = false;
        } catch (err) {
          return runner(self.b, right(self, "Error opening Xhr: " + err.message)), void notify(self, err);
        }
        data = id || "";
        var r = new Vector(self.headers);
        if (styles) {
          (function(obj, fn) {
            if (obj.forEach && "function" == typeof obj.forEach) {
              obj.forEach(fn, void 0);
            } else {
              if (isArray(obj) || isString(obj)) {
                equal(obj, fn, void 0);
              } else {
                var result = reduce(obj);
                var a = split(obj);
                var az = a.length;
                /** @type {number} */
                var i = 0;
                for (; i < az; i++) {
                  fn.call(void 0, a[i], result && result[i], obj);
                }
              }
            }
          })(styles, function(t, e) {
            r.set(e, t);
          });
        }
        styles = function(object) {
          t: {
            /** @type {function(string): ?} */
            var callback = headersForEvaluation;
            var length = object.length;
            var arraylike = isString(object) ? object.split("") : object;
            /** @type {number} */
            var index = 0;
            for (; index < length; index++) {
              if (index in arraylike && callback.call(void 0, arraylike[index], index, object)) {
                /** @type {number} */
                callback = index;
                break t;
              }
            }
            /** @type {number} */
            callback = -1;
          }
          return 0 > callback ? null : isString(object) ? object.charAt(callback) : object[callback];
        }(r.U());
        id = global.FormData && data instanceof global.FormData;
        if (!(!push(methods, type) || styles || id)) {
          r.set("Content-Type", "application/x-www-form-urlencoded;charset=utf-8");
        }
        r.forEach(function(b, n) {
          this.a.setRequestHeader(n, b);
        }, self);
        if (self.o) {
          self.a.responseType = self.o;
        }
        if ("withCredentials" in self.a && self.a.withCredentials !== self.O) {
          self.a.withCredentials = self.O;
        }
        try {
          hide(self);
          if (0 < self.g) {
            self.v = function(sweep) {
              return window && apply(9) && "number" == typeof sweep.timeout && void 0 !== sweep.ontimeout;
            }(self.a);
            runner(self.b, right(self, "Will abort after " + self.g + "ms if incomplete, xhr2 " + self.v));
            if (self.v) {
              self.a.timeout = self.g;
              self.a.ontimeout = bind(self.Fa, self);
            } else {
              self.s = filter(self.Fa, self.g, self);
            }
          }
          runner(self.b, right(self, "Sending request"));
          /** @type {boolean} */
          self.j = true;
          self.a.send(data);
          /** @type {boolean} */
          self.j = false;
        } catch (err) {
          runner(self.b, right(self, "Send error: " + err.message));
          notify(self, err);
        }
      }
      /**
       * @param {string} header
       * @return {?}
       */
      function headersForEvaluation(header) {
        return "content-type" == header.toLowerCase();
      }
      /**
       * @param {!Object} e
       * @param {number} d
       * @return {undefined}
       */
      function notify(e, d) {
        /** @type {boolean} */
        e.c = false;
        if (e.a) {
          /** @type {boolean} */
          e.f = true;
          e.a.abort();
          /** @type {boolean} */
          e.f = false;
        }
        /** @type {number} */
        e.h = d;
        ready(e);
        abort(e);
      }
      /**
       * @param {!Object} exports
       * @return {undefined}
       */
      function ready(exports) {
        if (!exports.G) {
          /** @type {boolean} */
          exports.G = true;
          exports.dispatchEvent("complete");
          exports.dispatchEvent("error");
        }
      }
      /**
       * @param {!Object} self
       * @return {undefined}
       */
      function next(self) {
        if (self.c && void 0 !== Directory) {
          if (self.B[1] && 4 == close(self) && 2 == up(self)) {
            runner(self.b, right(self, "Local request error detected and ignored"));
          } else {
            if (self.j && 4 == close(self)) {
              filter(self.Db, 0, self);
            } else {
              if (self.dispatchEvent("readystatechange"), 4 == close(self)) {
                runner(self.b, right(self, "Request complete"));
                /** @type {boolean} */
                self.c = false;
                try {
                  var return_block;
                  var n = up(self);
                  t: {
                    switch(n) {
                      case 200:
                      case 201:
                      case 202:
                      case 204:
                      case 206:
                      case 304:
                      case 1223:
                        /** @type {boolean} */
                        var block = true;
                        break t;
                      default:
                        /** @type {boolean} */
                        block = false;
                    }
                  }
                  if (!(return_block = block)) {
                    var block;
                    if (block = 0 === n) {
                      /** @type {(null|string)} */
                      var scheme = String(self.l).match(re2)[1] || null;
                      if (!scheme && global.self && global.self.location) {
                        var protocol = global.self.location.protocol;
                        scheme = protocol.substr(0, protocol.length - 1);
                      }
                      /** @type {boolean} */
                      block = !alpha.test(scheme ? scheme.toLowerCase() : "");
                    }
                    /** @type {boolean} */
                    return_block = block;
                  }
                  if (return_block) {
                    self.dispatchEvent("complete");
                    self.dispatchEvent("success");
                  } else {
                    try {
                      var generatorMark = 2 < close(self) ? self.a.statusText : "";
                    } catch (controlFlowAction) {
                      runner(self.b, "Can not get status: " + controlFlowAction.message);
                      /** @type {string} */
                      generatorMark = "";
                    }
                    /** @type {string} */
                    self.h = generatorMark + " [" + up(self) + "]";
                    ready(self);
                  }
                } finally {
                  abort(self);
                }
              }
            }
          }
        }
      }
      /**
       * @param {!Object} root
       * @param {boolean} verbose
       * @return {undefined}
       */
      function abort(root, verbose) {
        if (root.a) {
          hide(root);
          var node = root.a;
          /** @type {(function(): undefined|null)} */
          var callback = root.B[0] ? noop : null;
          /** @type {null} */
          root.a = null;
          /** @type {null} */
          root.B = null;
          if (!verbose) {
            root.dispatchEvent("ready");
          }
          try {
            /** @type {(function(): undefined|null)} */
            node.onreadystatechange = callback;
          } catch (controlFlowAction) {
            if (root = root.b) {
              root.log(str, "Problem encountered resetting onreadystatechange: " + controlFlowAction.message, void 0);
            }
          }
        }
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function hide(data) {
        if (data.a && data.v) {
          /** @type {null} */
          data.a.ontimeout = null;
        }
        if (data.s) {
          global.clearTimeout(data.s);
          /** @type {null} */
          data.s = null;
        }
      }
      /**
       * @param {!Object} instance
       * @return {?}
       */
      function close(instance) {
        return instance.a ? instance.a.readyState : 0;
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function up(obj) {
        try {
          return 2 < close(obj) ? obj.a.status : -1;
        } catch (t) {
          return -1;
        }
      }
      /**
       * @param {!Object} node
       * @param {string} p
       * @return {?}
       */
      function right(node, p) {
        return p + " [" + node.N + " " + node.l + " " + up(node) + "]";
      }
      /**
       * @param {number} v
       * @param {string} s
       * @return {undefined}
       */
      function m(v, s) {
        /** @type {!Array} */
        this.g = [];
        /** @type {number} */
        this.v = v;
        this.s = s || null;
        /** @type {boolean} */
        this.f = this.a = false;
        this.c = void 0;
        /** @type {boolean} */
        this.u = this.B = this.j = false;
        /** @type {number} */
        this.h = 0;
        /** @type {null} */
        this.b = null;
        /** @type {number} */
        this.l = 0;
      }
      /**
       * @param {!Object} value
       * @param {boolean} str
       * @param {string} a
       * @return {undefined}
       */
      function w(value, str, a) {
        /** @type {boolean} */
        value.a = true;
        /** @type {string} */
        value.c = a;
        /** @type {boolean} */
        value.f = !str;
        main(value);
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function show(data) {
        if (data.a) {
          if (!data.u) {
            throw new Notification(data);
          }
          /** @type {boolean} */
          data.u = false;
        }
      }
      /**
       * @param {!Object} options
       * @param {!Object} content
       * @param {!Function} action
       * @param {!Object} result
       * @return {undefined}
       */
      function after(options, content, action, result) {
        options.g.push([content, action, result]);
        if (options.a) {
          main(options);
        }
      }
      /**
       * @param {!Object} rgb
       * @return {?}
       */
      function validateRGBAArr(rgb) {
        return convertToRgbInt(rgb.g, function(solData) {
          return isNaN(solData[1]);
        });
      }
      /**
       * @param {!Object} c
       * @return {undefined}
       */
      function main(c) {
        if (c.h && c.a && validateRGBAArr(c)) {
          var a = c.h;
          var item = elements[a];
          if (item) {
            global.clearTimeout(item.a);
            delete elements[a];
          }
          /** @type {number} */
          c.h = 0;
        }
        if (c.b) {
          c.b.l--;
          delete c.b;
        }
        a = c.c;
        /** @type {boolean} */
        var err = item = false;
        for (; c.g.length && !c.j;) {
          var list = c.g.shift();
          var next = list[0];
          var cur = list[1];
          if (list = list[2], next = c.f ? cur : next) {
            try {
              var value = next.call(list || c.s, a);
              if (void 0 !== value) {
                c.f = c.f && (value == a || value instanceof Error);
                c.c = a = value;
              }
              if (match(a) || "function" == typeof global.Promise && a instanceof global.Promise) {
                /** @type {boolean} */
                err = true;
                /** @type {boolean} */
                c.j = true;
              }
            } catch (b) {
              a = b;
              /** @type {boolean} */
              c.f = true;
              if (!validateRGBAArr(c)) {
                /** @type {boolean} */
                item = true;
              }
            }
          }
        }
        c.c = a;
        if (err) {
          value = bind(c.o, c, true);
          err = bind(c.o, c, false);
          if (a instanceof m) {
            after(a, value, err);
            /** @type {boolean} */
            a.B = true;
          } else {
            a.then(value, err);
          }
        }
        if (item) {
          a = new Person(a);
          elements[a.a] = a;
          c.h = a.a;
        }
      }
      /**
       * @return {undefined}
       */
      function Notification() {
        Constructor.call(this);
      }
      /**
       * @return {undefined}
       */
      function Tag() {
        Constructor.call(this);
      }
      /**
       * @param {number} b
       * @return {undefined}
       */
      function Person(b) {
        this.a = global.setTimeout(bind(this.c, this), 0);
        /** @type {number} */
        this.b = b;
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function fetch(value) {
        var opts = {};
        var parent = opts.document || document;
        var view = clean(value);
        /** @type {!Element} */
        var el = document.createElement("SCRIPT");
        var name = {
          Fb : el,
          Fa : void 0
        };
        var input = new m(isUpgradable, name);
        /** @type {null} */
        var variable = null;
        var renewTokenIn = null != opts.timeout ? opts.timeout : 5E3;
        return 0 < renewTokenIn && (variable = window.setTimeout(function() {
          runTest(el, true);
          var newState = new Response(defaultPath, "Timeout reached for loading script " + view);
          show(input);
          w(input, false, newState);
        }, renewTokenIn), name.Fa = variable), el.onload = el.onreadystatechange = function() {
          if (!(el.readyState && "loaded" != el.readyState && "complete" != el.readyState)) {
            runTest(el, opts.bd || false, variable);
            input.D();
          }
        }, el.onerror = function() {
          runTest(el, true, variable);
          var response = new Response(rawResponse, "Error while loading script " + view);
          show(input);
          w(input, false, response);
        }, log(name = opts.attributes || {}, {
          type : "text/javascript",
          charset : "UTF-8"
        }), load(el, name), el.src = clean(value), function(context) {
          var sourceObj;
          return (sourceObj = (context || document).getElementsByTagName("HEAD")) && 0 != sourceObj.length ? sourceObj[0] : context.documentElement;
        }(parent).appendChild(el), input;
      }
      /**
       * @return {undefined}
       */
      function isUpgradable() {
        if (this && this.Fb) {
          var script = this.Fb;
          if (script && "SCRIPT" == script.tagName) {
            runTest(script, true, this.Fa);
          }
        }
      }
      /**
       * @param {!Object} script
       * @param {boolean} id
       * @param {!Object} n
       * @return {undefined}
       */
      function runTest(script, id, n) {
        if (null != n) {
          global.clearTimeout(n);
        }
        /** @type {function(): undefined} */
        script.onload = noop;
        /** @type {function(): undefined} */
        script.onerror = noop;
        /** @type {function(): undefined} */
        script.onreadystatechange = noop;
        if (id) {
          window.setTimeout(function() {
            if (script && script.parentNode) {
              script.parentNode.removeChild(script);
            }
          }, 0);
        }
      }
      /**
       * @param {string} code
       * @param {string} statusCode
       * @return {undefined}
       */
      function Response(code, statusCode) {
        /** @type {string} */
        var message = "Jsloader error (code #" + code + ")";
        if (statusCode) {
          /** @type {string} */
          message = message + (": " + statusCode);
        }
        Constructor.call(this, message);
        /** @type {string} */
        this.code = code;
      }
      /**
       * @param {!Function} f
       * @return {undefined}
       */
      function a(f) {
        /** @type {!Function} */
        this.f = f;
      }
      /**
       * @param {string} b
       * @param {string} a
       * @param {!Object} p
       * @return {undefined}
       */
      function Player(b, a, p) {
        if (this.b = b, b = a || {}, this.j = b.secureTokenEndpoint || "https://securetoken.googleapis.com/v1/token", this.l = b.secureTokenTimeout || EMPTY, this.f = clamp(b.secureTokenHeaders || header), this.g = b.firebaseEndpoint || "https://www.googleapis.com/identitytoolkit/v3/relyingparty/", this.h = b.firebaseTimeout || defaultPriority, this.a = clamp(b.firebaseHeaders || JSON_CONTENT_TYPE_HEADER), p && (this.a["X-Client-Version"] = p, this.f["X-Client-Version"] = p), p = "Node" == css(), 
        !(p = global.XMLHttpRequest || p && config.INTERNAL.node && config.INTERNAL.node.XMLHttpRequest) && !replace()) {
          throw new Buffer("internal-error", "The XMLHttpRequest compatibility library was not found.");
        }
        this.c = void 0;
        if (replace()) {
          this.c = new Worker(self);
        } else {
          if (getHeight()) {
            this.c = new a(p);
          } else {
            this.c = new cc;
          }
        }
      }
      /**
       * @param {(number|string)} p
       * @param {string} a
       * @return {undefined}
       */
      function f3(p, a) {
        if (a) {
          /** @type {string} */
          p.a["X-Firebase-Locale"] = a;
        } else {
          delete p.a["X-Firebase-Locale"];
        }
      }
      /**
       * @param {!Object} options
       * @param {?} extraHeaders
       * @return {undefined}
       */
      function reply(options, extraHeaders) {
        if (extraHeaders) {
          options.a["X-Client-Version"] = extraHeaders;
          options.f["X-Client-Version"] = extraHeaders;
        } else {
          delete options.a["X-Client-Version"];
          delete options.f["X-Client-Version"];
        }
      }
      /**
       * @param {!Object} t
       * @param {string} e
       * @param {!Function} context
       * @param {string} path
       * @param {?} vars
       * @param {?} target
       * @param {?} buildResults
       * @return {undefined}
       */
      function install(t, e, context, path, vars, target, buildResults) {
        if (function() {
          var time = end();
          return !((time = detect(time) != months ? null : (time = time.match(/\sChrome\/(\d+)/i)) && 2 == time.length ? parseInt(time[1], 10) : null) && 30 > time || window && element && !(9 < element));
        }() || replace()) {
          t = bind(t.o, t);
        } else {
          if (!serverRoutes) {
            serverRoutes = new Promise(function(onDestroyed, apexdist) {
              !function(destroyed, c) {
                if (((window.gapi || {}).client || {}).request) {
                  destroyed();
                } else {
                  /**
                   * @return {undefined}
                   */
                  global[evsImportName] = function() {
                    if (((window.gapi || {}).client || {}).request) {
                      destroyed();
                    } else {
                      c(Error("CORS_UNSUPPORTED"));
                    }
                  };
                  var res = check(row, {
                    onload : evsImportName
                  });
                  !function(n, func) {
                    after(n, null, func, void 0);
                  }(fetch(res), function() {
                    c(Error("CORS_UNSUPPORTED"));
                  });
                }
              }(onDestroyed, apexdist);
            });
          }
          t = bind(t.u, t);
        }
        t(e, context, path, vars, target, buildResults);
      }
      /**
       * @param {!Object} val
       * @return {undefined}
       */
      function T(val) {
        if (!validator.test(val.email)) {
          throw new Buffer("invalid-email");
        }
      }
      /**
       * @param {!Object} value
       * @return {undefined}
       */
      function y(value) {
        if ("email" in value) {
          T(value);
        }
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function getDate(data) {
        if (!data[i]) {
          throw new Buffer("internal-error");
        }
      }
      /**
       * @param {!Object} options
       * @return {undefined}
       */
      function handleError(options) {
        if (options.phoneNumber || options.temporaryProof) {
          if (!options.phoneNumber || !options.temporaryProof) {
            throw new Buffer("internal-error");
          }
        } else {
          if (!options.sessionInfo) {
            throw new Buffer("missing-verification-id");
          }
          if (!options.code) {
            throw new Buffer("missing-verification-code");
          }
        }
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function Request(data) {
        if (!data.requestUri || !data.sessionId && !data.postBody) {
          throw new Buffer("internal-error");
        }
      }
      /**
       * @param {!Object} d
       * @return {undefined}
       */
      function J(d) {
        /** @type {null} */
        var result = null;
        if (d.needConfirmation ? (d.code = "account-exists-with-different-credential", result = serialize(d)) : "FEDERATED_USER_ID_ALREADY_LINKED" == d.errorMessage ? (d.code = "credential-already-in-use", result = serialize(d)) : "EMAIL_EXISTS" == d.errorMessage ? (d.code = "email-already-in-use", result = serialize(d)) : d.errorMessage && (result = $$(d.errorMessage)), result) {
          throw result;
        }
        if (!d[i]) {
          throw new Buffer("internal-error");
        }
      }
      /**
       * @param {!Object} key
       * @param {string} url
       * @return {?}
       */
      function getApi(key, url) {
        return url.returnIdpCredential = true, callback(key, subscription, url);
      }
      /**
       * @param {!Object} t
       * @param {!Object} v
       * @return {?}
       */
      function children(t, v) {
        return v.returnIdpCredential = true, callback(t, formats, v);
      }
      /**
       * @param {!Object} app
       * @param {!Object} options
       * @return {?}
       */
      function Collection(app, options) {
        return options.returnIdpCredential = true, options.autoCreate = false, callback(app, types, options);
      }
      /**
       * @param {!Object} type
       * @return {undefined}
       */
      function testExtBufferArray(type) {
        if (!type.oobCode) {
          throw new Buffer("invalid-action-code");
        }
      }
      /**
       * @param {!Object} target
       * @param {!Object} obj
       * @param {!Object} n
       * @return {?}
       */
      function callback(target, obj, n) {
        if (!function(keys, array) {
          if (!array || !array.length) {
            return true;
          }
          if (!keys) {
            return false;
          }
          /** @type {number} */
          var i = 0;
          for (; i < array.length; i++) {
            var key = keys[array[i]];
            if (void 0 === key || null === key || "" === key) {
              return false;
            }
          }
          return true;
        }(n, obj.T)) {
          return cb(new Buffer("internal-error"));
        }
        var result;
        var method = obj.Cb || "POST";
        return resolve(n).then(obj.A).then(function() {
          return obj.R && (n.returnSecureToken = true), function(value, distance, url, c, modstatus, canCreateDiscussions) {
            var a = getValue(value.g + distance);
            func(a, "key", value.b);
            if (canCreateDiscussions) {
              func(a, "cb", min().toString());
            }
            /** @type {boolean} */
            var _linkTypeIsUrl = "GET" == url;
            if (_linkTypeIsUrl) {
              var j;
              for (j in c) {
                if (c.hasOwnProperty(j)) {
                  func(a, j, c[j]);
                }
              }
            }
            return new Promise(function(enterEventHandler, cb) {
              install(value, a.toString(), function(e) {
                if (e) {
                  if (e.error) {
                    cb(errorHandler(e, modstatus || {}));
                  } else {
                    enterEventHandler(e);
                  }
                } else {
                  cb(new Buffer("network-request-failed"));
                }
              }, url, _linkTypeIsUrl ? void 0 : addItem($getDRtoDL(c)), value.a, value.h.get());
            });
          }(target, obj.endpoint, method, n, obj.Tb, obj.pb || false);
        }).then(function(textCode) {
          return result = textCode;
        }).then(obj.J).then(function() {
          if (!obj.ba) {
            return result;
          }
          if (!(obj.ba in result)) {
            throw new Buffer("internal-error");
          }
          return result[obj.ba];
        });
      }
      /**
       * @param {string} param
       * @return {?}
       */
      function $$(param) {
        return errorHandler({
          error : {
            errors : [{
              message : param
            }],
            code : 400,
            message : param
          }
        });
      }
      /**
       * @param {!Object} data
       * @param {!Object} err
       * @return {?}
       */
      function errorHandler(data, err) {
        var name = (data.error && data.error.errors && data.error.errors[0] || {}).reason || "";
        var obj = {
          keyInvalid : "invalid-api-key",
          ipRefererBlocked : "app-not-authorized"
        };
        if (name = obj[name] ? new Buffer(obj[name]) : null) {
          return name;
        }
        var key;
        for (key in name = data.error && data.error.message || "", log(obj = {
          INVALID_CUSTOM_TOKEN : "invalid-custom-token",
          CREDENTIAL_MISMATCH : "custom-token-mismatch",
          MISSING_CUSTOM_TOKEN : "internal-error",
          INVALID_IDENTIFIER : "invalid-email",
          MISSING_CONTINUE_URI : "internal-error",
          INVALID_EMAIL : "invalid-email",
          INVALID_PASSWORD : "wrong-password",
          USER_DISABLED : "user-disabled",
          MISSING_PASSWORD : "internal-error",
          EMAIL_EXISTS : "email-already-in-use",
          PASSWORD_LOGIN_DISABLED : "operation-not-allowed",
          INVALID_IDP_RESPONSE : "invalid-credential",
          FEDERATED_USER_ID_ALREADY_LINKED : "credential-already-in-use",
          INVALID_MESSAGE_PAYLOAD : "invalid-message-payload",
          INVALID_RECIPIENT_EMAIL : "invalid-recipient-email",
          INVALID_SENDER : "invalid-sender",
          EMAIL_NOT_FOUND : "user-not-found",
          EXPIRED_OOB_CODE : "expired-action-code",
          INVALID_OOB_CODE : "invalid-action-code",
          MISSING_OOB_CODE : "internal-error",
          CREDENTIAL_TOO_OLD_LOGIN_AGAIN : "requires-recent-login",
          INVALID_ID_TOKEN : "invalid-user-token",
          TOKEN_EXPIRED : "user-token-expired",
          USER_NOT_FOUND : "user-token-expired",
          CORS_UNSUPPORTED : "cors-unsupported",
          DYNAMIC_LINK_NOT_ACTIVATED : "dynamic-link-not-activated",
          INVALID_APP_ID : "invalid-app-id",
          TOO_MANY_ATTEMPTS_TRY_LATER : "too-many-requests",
          WEAK_PASSWORD : "weak-password",
          OPERATION_NOT_ALLOWED : "operation-not-allowed",
          USER_CANCELLED : "user-cancelled",
          CAPTCHA_CHECK_FAILED : "captcha-check-failed",
          INVALID_APP_CREDENTIAL : "invalid-app-credential",
          INVALID_CODE : "invalid-verification-code",
          INVALID_PHONE_NUMBER : "invalid-phone-number",
          INVALID_SESSION_INFO : "invalid-verification-id",
          INVALID_TEMPORARY_PROOF : "invalid-credential",
          MISSING_APP_CREDENTIAL : "missing-app-credential",
          MISSING_CODE : "missing-verification-code",
          MISSING_PHONE_NUMBER : "missing-phone-number",
          MISSING_SESSION_INFO : "missing-verification-id",
          QUOTA_EXCEEDED : "quota-exceeded",
          SESSION_EXPIRED : "code-expired",
          INVALID_CONTINUE_URI : "invalid-continue-uri",
          MISSING_ANDROID_PACKAGE_NAME : "missing-android-pkg-name",
          MISSING_IOS_BUNDLE_ID : "missing-ios-bundle-id",
          UNAUTHORIZED_DOMAIN : "unauthorized-continue-uri",
          INVALID_OAUTH_CLIENT_ID : "invalid-oauth-client-id",
          INVALID_CERT_HASH : "invalid-cert-hash"
        }, err || {}), err = (err = name.match(/^[^\s]+\s*:\s*(.*)$/)) && 1 < err.length ? err[1] : void 0, obj) {
          if (0 === name.indexOf(key)) {
            return new Buffer(obj[key], err);
          }
        }
        return !err && data && (err = parse(data)), new Buffer("internal-error", err);
      }
      /**
       * @param {string} req
       * @return {?}
       */
      function join(req) {
        var i;
        for (i in reqs) {
          if (reqs[i].id === req) {
            return {
              firebaseEndpoint : (req = reqs[i]).Ya,
              secureTokenEndpoint : req.eb
            };
          }
        }
        return null;
      }
      /**
       * @param {number} b
       * @return {undefined}
       */
      function injectBrowser(b) {
        /** @type {number} */
        this.b = b;
        /** @type {null} */
        this.a = null;
        this.ab = function(doc) {
          return (offset || (offset = (new Promise(function(doneHandler, callback) {
            /**
             * @return {undefined}
             */
            function requestXHR() {
              keydown();
              stringify("gapi.load")("gapi.iframes", {
                callback : doneHandler,
                ontimeout : function() {
                  keydown();
                  callback(Error("Network Error"));
                },
                timeout : clickedGroup.get()
              });
            }
            if (stringify("gapi.iframes.Iframe")) {
              doneHandler();
            } else {
              if (stringify("gapi.load")) {
                requestXHR();
              } else {
                /** @type {string} */
                var name = "__iframefcb" + Math.floor(1E6 * Math.random()).toString();
                /**
                 * @return {undefined}
                 */
                global[name] = function() {
                  if (stringify("gapi.load")) {
                    requestXHR();
                  } else {
                    callback(Error("Network Error"));
                  }
                };
                resolve(fetch(name = check(left, {
                  onload : name
                }))).m(function() {
                  callback(Error("Network Error"));
                });
              }
            }
          })).m(function(string) {
            throw offset = null, string;
          }))).then(function() {
            return new Promise(function(opt_cb, callback) {
              stringify("gapi.iframes.getContext")().open({
                where : document.body,
                url : doc.b,
                messageHandlersFilter : stringify("gapi.iframes.CROSS_ORIGIN_IFRAMES_FILTER"),
                attributes : {
                  style : {
                    position : "absolute",
                    top : "-100px",
                    width : "1px",
                    height : "1px"
                  }
                },
                dontclear : true
              }, function(a) {
                /**
                 * @return {undefined}
                 */
                function cb() {
                  clearTimeout(autoResumeTimer);
                  opt_cb();
                }
                /** @type {!Object} */
                doc.a = a;
                doc.a.restyle({
                  setHideOnLeave : false
                });
                /** @type {number} */
                var autoResumeTimer = setTimeout(function() {
                  callback(Error("Network Error"));
                }, readerAPI.get());
                a.ping(cb).then(cb, function() {
                  callback(Error("Network Error"));
                });
              });
            });
          });
        }(this);
      }
      /**
       * @param {number} b
       * @param {string} c
       * @param {number} h
       * @return {undefined}
       */
      function Color(b, c, h) {
        /** @type {number} */
        this.j = b;
        /** @type {string} */
        this.g = c;
        /** @type {number} */
        this.h = h;
        /** @type {null} */
        this.f = null;
        this.a = setColor(this.j, "/__/auth/iframe");
        func(this.a, "apiKey", this.g);
        func(this.a, "appName", this.h);
        /** @type {null} */
        this.b = null;
        /** @type {!Array} */
        this.c = [];
      }
      /**
       * @param {(Object|string)} options
       * @param {number} array
       * @param {number} count
       * @param {number} level
       * @param {number} angle
       * @return {undefined}
       */
      function constructor(options, array, count, level, angle) {
        /** @type {(Object|string)} */
        this.o = options;
        /** @type {number} */
        this.u = array;
        /** @type {number} */
        this.c = count;
        /** @type {number} */
        this.l = level;
        /** @type {null} */
        this.h = this.g = this.j = null;
        /** @type {number} */
        this.a = angle;
        /** @type {null} */
        this.f = null;
      }
      /**
       * @param {?} settings
       * @return {?}
       */
      function concat(settings) {
        try {
          return config.app(settings).auth().Ka();
        } catch (t) {
          return [];
        }
      }
      /**
       * @param {string} a
       * @param {!Function} f
       * @param {number} val
       * @param {string} c
       * @param {string} width
       * @return {undefined}
       */
      function calc(a, f, val, c, width) {
        /** @type {string} */
        this.u = a;
        /** @type {!Function} */
        this.f = f;
        /** @type {number} */
        this.b = val;
        this.c = c || null;
        this.h = width || null;
        /** @type {null} */
        this.o = this.s = this.v = null;
        /** @type {!Array} */
        this.g = [];
        /** @type {null} */
        this.l = this.a = null;
      }
      /**
       * @param {!Object} element
       * @return {?}
       */
      function postLink(element) {
        var c = updatePresenterWindow();
        return function(t) {
          return callback(t, packet, {}).then(function(canCreateDiscussions) {
            return canCreateDiscussions.authorizedDomains || [];
          });
        }(element).then(function(tokens2) {
          t: {
            var a = getValue(c);
            var l = a.c;
            a = a.b;
            /** @type {number} */
            var i = 0;
            for (; i < tokens2.length; i++) {
              var b = tokens2[i];
              var c = a;
              var t = l;
              if (0 == b.indexOf("chrome-extension://") ? c = getValue(b).b == c && "chrome-extension" == t : "http" != t && "https" != t ? c = false : $jsilxna.test(b) ? c = c == b : (b = b.split(".").join("\\."), c = (new RegExp("^(.+\\." + b + "|" + b + ")$", "i")).test(c)), c) {
                /** @type {boolean} */
                tokens2 = true;
                break t;
              }
            }
            /** @type {boolean} */
            tokens2 = false;
          }
          if (!tokens2) {
            throw new ui(updatePresenterWindow());
          }
        });
      }
      /**
       * @param {!Object} options
       * @return {?}
       */
      function message(options) {
        return options.l ? options.l : (options.l = play().then(function() {
          if (!options.s) {
            var format = options.c;
            var t = options.h;
            var deps = concat(options.b);
            var c = new Color(options.u, options.f, options.b);
            c.f = format;
            c.b = t;
            c.c = require(deps || []);
            options.s = c.toString();
          }
          options.j = new injectBrowser(options.s);
          (function(options) {
            if (!options.j) {
              throw Error("IfcHandler must be initialized!");
            }
            !function(data, cartoLayer) {
              data.ab.then(function() {
                data.a.register("authEvent", cartoLayer, stringify("gapi.iframes.CROSS_ORIGIN_IFRAMES_FILTER"));
              });
            }(options.j, function(e) {
              var i = {};
              if (e && e.authEvent) {
                /** @type {boolean} */
                var res = false;
                e = stop(e.authEvent);
                /** @type {number} */
                i = 0;
                for (; i < options.g.length; i++) {
                  res = options.g[i](e) || res;
                }
                return (i = {}).status = res ? "ACK" : "ERROR", resolve(i);
              }
              return i.status = "ERROR", resolve(i);
            });
          })(options);
        }), options.l);
      }
      /**
       * @param {!Object} data
       * @return {?}
       */
      function reset(data) {
        return data.o || (data.v = data.c ? success(data.c, concat(data.b)) : null, data.o = new Player(data.f, join(data.h), data.v)), data.o;
      }
      /**
       * @param {!Object} value
       * @param {string} type
       * @param {!Function} string
       * @param {string} template
       * @param {!Object} key
       * @param {number} item
       * @param {string} data
       * @param {number} element
       * @param {number} n
       * @param {!Function} s
       * @return {?}
       */
      function format(value, type, string, template, key, item, data, element, n, s) {
        return (value = new constructor(value, type, string, template, key)).j = item, value.g = data, value.h = element, value.b = clamp(n || null), value.f = s, value.toString();
      }
      /**
       * @param {string} a
       * @return {undefined}
       */
      function h(a) {
        if (this.a = a || config.INTERNAL.reactNative && config.INTERNAL.reactNative.AsyncStorage, !this.a) {
          throw new Buffer("internal-error", "The React Native compatibility library was not found.");
        }
        /** @type {string} */
        this.type = "asyncStorage";
      }
      /**
       * @return {undefined}
       */
      function getOverlayStyles() {
        if (!iOSversion()) {
          throw new Buffer("web-storage-unsupported");
        }
        this.f = {};
        /** @type {!Array} */
        this.a = [];
        /** @type {number} */
        this.b = 0;
        this.g = global.indexedDB;
        /** @type {string} */
        this.type = "indexedDB";
      }
      /**
       * @param {!Object} opts
       * @return {?}
       */
      function openDB(opts) {
        return new Promise(function(resolve, callback) {
          var dbOpen = opts.g.open("firebaseLocalStorageDb", 1);
          /**
           * @param {!Object} e
           * @return {undefined}
           */
          dbOpen.onerror = function(e) {
            try {
              e.preventDefault();
            } catch (t) {
            }
            callback(Error(e.target.error));
          };
          /**
           * @param {string} event
           * @return {undefined}
           */
          dbOpen.onupgradeneeded = function(event) {
            event = event.target.result;
            try {
              event.createObjectStore("firebaseLocalStorage", {
                keyPath : "fbase_key"
              });
            } catch (identifierPositions) {
              callback(identifierPositions);
            }
          };
          /**
           * @param {string} response
           * @return {undefined}
           */
          dbOpen.onsuccess = function(response) {
            if ((response = response.target.result).objectStoreNames.contains("firebaseLocalStorage")) {
              resolve(response);
            } else {
              (function(self) {
                return new Promise(function(removeDoneCallback, callback) {
                  var idb_req = self.g.deleteDatabase("firebaseLocalStorageDb");
                  /**
                   * @return {undefined}
                   */
                  idb_req.onsuccess = function() {
                    removeDoneCallback();
                  };
                  /**
                   * @param {!Object} x
                   * @return {undefined}
                   */
                  idb_req.onerror = function(x) {
                    callback(Error(x.target.error));
                  };
                });
              })(opts).then(function() {
                return openDB(opts);
              }).then(function(resolveParameter) {
                resolve(resolveParameter);
              }).m(function(identifierPositions) {
                callback(identifierPositions);
              });
            }
          };
        });
      }
      /**
       * @param {!Window} o
       * @return {?}
       */
      function defined(o) {
        return o.h || (o.h = openDB(o)), o.h;
      }
      /**
       * @return {?}
       */
      function iOSversion() {
        try {
          return !!global.indexedDB;
        } catch (t) {
          return false;
        }
      }
      /**
       * @param {!IDBTransaction} index
       * @return {?}
       */
      function removeChild(index) {
        return index.objectStore("firebaseLocalStorage");
      }
      /**
       * @param {!Object} val
       * @param {string} writable
       * @return {?}
       */
      function setTimeout(val, writable) {
        return val.transaction(["firebaseLocalStorage"], writable ? "readwrite" : "readonly");
      }
      /**
       * @param {!Object} req
       * @return {?}
       */
      function promisifyRequest(req) {
        return new Promise(function(successFunc, callback) {
          /**
           * @param {!Object} evt
           * @return {undefined}
           */
          req.onsuccess = function(evt) {
            if (evt && evt.target) {
              successFunc(evt.target.result);
            } else {
              successFunc();
            }
          };
          /**
           * @param {!Object} e
           * @return {undefined}
           */
          req.onerror = function(e) {
            callback(Error(e.target.errorCode));
          };
        });
      }
      /**
       * @param {number} value
       * @return {undefined}
       */
      function listener(value) {
        var attr = this;
        /** @type {null} */
        var byteBuffer = null;
        /** @type {!Array} */
        this.a = [];
        /** @type {string} */
        this.type = "indexedDB";
        /** @type {number} */
        this.c = value;
        this.b = resolve().then(function() {
          return iOSversion() ? (_button || (_button = new getOverlayStyles), (byteBuffer = _button).set("__sak", "!").then(function() {
            return byteBuffer.get("__sak");
          }).then(function(canCreateDiscussions) {
            if ("!" !== canCreateDiscussions) {
              throw Error("indexedDB not supported!");
            }
            return byteBuffer.P("__sak");
          }).then(function() {
            return byteBuffer;
          }).m(function() {
            return attr.c;
          })) : attr.c;
        }).then(function(s) {
          return attr.type = s.type, s.Y(function(notifications) {
            equal(attr.a, function(saveNotifs) {
              saveNotifs(notifications);
            });
          }), s;
        });
      }
      /**
       * @return {undefined}
       */
      function Converter() {
        this.a = {};
        /** @type {string} */
        this.type = "inMemory";
      }
      /**
       * @return {undefined}
       */
      function _init() {
        if (!function() {
          /** @type {boolean} */
          var result = "Node" == css();
          if (!(result = test_local_storage() || result && config.INTERNAL.node && config.INTERNAL.node.localStorage)) {
            return false;
          }
          try {
            return result.setItem("__sak", "1"), result.removeItem("__sak"), true;
          } catch (t) {
            return false;
          }
        }()) {
          if ("Node" == css()) {
            throw new Buffer("internal-error", "The LocalStorage compatibility library was not found.");
          }
          throw new Buffer("web-storage-unsupported");
        }
        this.a = test_local_storage() || config.INTERNAL.node.localStorage;
        /** @type {string} */
        this.type = "localStorage";
      }
      /**
       * @return {?}
       */
      function test_local_storage() {
        try {
          var storage = global.localStorage;
          var i = removeItem();
          return storage && (storage.setItem(i, "1"), storage.removeItem(i)), storage;
        } catch (t) {
          return null;
        }
      }
      /**
       * @return {undefined}
       */
      function effect() {
        /** @type {string} */
        this.type = "nullStorage";
      }
      /**
       * @return {undefined}
       */
      function complete() {
        if (!function() {
          /** @type {boolean} */
          var result = "Node" == css();
          if (!(result = removeData() || result && config.INTERNAL.node && config.INTERNAL.node.sessionStorage)) {
            return false;
          }
          try {
            return result.setItem("__sak", "1"), result.removeItem("__sak"), true;
          } catch (t) {
            return false;
          }
        }()) {
          if ("Node" == css()) {
            throw new Buffer("internal-error", "The SessionStorage compatibility library was not found.");
          }
          throw new Buffer("web-storage-unsupported");
        }
        this.a = removeData() || config.INTERNAL.node.sessionStorage;
        /** @type {string} */
        this.type = "sessionStorage";
      }
      /**
       * @return {?}
       */
      function removeData() {
        try {
          var storage = global.sessionStorage;
          var i = removeItem();
          return storage && (storage.setItem(i, "1"), storage.removeItem(i)), storage;
        } catch (t) {
          return null;
        }
      }
      /**
       * @return {undefined}
       */
      function _initialize() {
        /** @type {boolean} */
        var o = !(process(end()) || !click());
        var x = query();
        var l = detectPrivateMode();
        /** @type {boolean} */
        this.o = o;
        this.h = x;
        this.l = l;
        this.a = {};
        if (!tObj) {
          tObj = new function() {
            var exports = {};
            exports.Browser = mem;
            exports.Node = qr;
            exports.ReactNative = rtn;
            exports.Worker = q;
            this.a = exports[css()];
          };
        }
        o = tObj;
        try {
          this.g = !retry() && applyChange() || !global.indexedDB ? new o.a.w : new listener(replace() ? new Converter : new o.a.w);
        } catch (t) {
          this.g = new Converter;
          /** @type {boolean} */
          this.h = true;
        }
        try {
          this.j = new o.a.Pa;
        } catch (t) {
          this.j = new Converter;
        }
        this.u = new Converter;
        this.f = bind(this.Kb, this);
        this.b = {};
      }
      /**
       * @return {?}
       */
      function random() {
        return Ns || (Ns = new _initialize), Ns;
      }
      /**
       * @param {!Object} c
       * @param {string} type
       * @return {?}
       */
      function jQuery(c, type) {
        switch(type) {
          case "session":
            return c.j;
          case "none":
            return c.u;
          default:
            return c.g;
        }
      }
      /**
       * @param {!Object} t
       * @param {string} s
       * @return {?}
       */
      function trim(t, s) {
        return "firebase:" + t.name + (s ? ":" + s : "");
      }
      /**
       * @param {!Object} f
       * @param {!Window} e
       * @param {string} key
       * @return {?}
       */
      function insertText(f, e, key) {
        return key = trim(e, key), "local" == e.w && (f.b[key] = null), jQuery(f, e.w).P(key);
      }
      /**
       * @param {!Object} o
       * @return {undefined}
       */
      function getKey(o) {
        if (o.c) {
          clearInterval(o.c);
          /** @type {null} */
          o.c = null;
        }
      }
      /**
       * @param {number} context
       * @param {number} value
       * @return {undefined}
       */
      function val(context, value) {
        /** @type {number} */
        this.b = -1;
        /** @type {number} */
        this.b = groupsize;
        /** @type {(Array|Uint8Array)} */
        this.f = global.Uint8Array ? new Uint8Array(this.b) : Array(this.b);
        /** @type {number} */
        this.g = this.c = 0;
        /** @type {!Array} */
        this.a = [];
        /** @type {number} */
        this.j = context;
        /** @type {number} */
        this.h = value;
        /** @type {(Array|Int32Array)} */
        this.l = global.Int32Array ? new Int32Array(64) : Array(64);
        if (!(void 0 !== descname)) {
          /** @type {(Array|Int32Array)} */
          descname = global.Int32Array ? new Int32Array(assetlaunched) : assetlaunched;
        }
        this.reset();
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function fmt(data) {
        var l = data.f;
        var n = data.l;
        /** @type {number} */
        var a = 0;
        /** @type {number} */
        var b = 0;
        for (; b < l.length;) {
          /** @type {number} */
          n[a++] = l[b] << 24 | l[b + 1] << 16 | l[b + 2] << 8 | l[b + 3];
          /** @type {number} */
          b = 4 * a;
        }
        /** @type {number} */
        l = 16;
        for (; 64 > l; l++) {
          /** @type {number} */
          b = 0 | n[l - 15];
          /** @type {number} */
          a = 0 | n[l - 2];
          /** @type {number} */
          var controlsCount = (0 | n[l - 16]) + ((b >>> 7 | b << 25) ^ (b >>> 18 | b << 14) ^ b >>> 3) | 0;
          /** @type {number} */
          var index = (0 | n[l - 7]) + ((a >>> 17 | a << 15) ^ (a >>> 19 | a << 13) ^ a >>> 10) | 0;
          /** @type {number} */
          n[l] = controlsCount + index | 0;
        }
        /** @type {number} */
        a = 0 | data.a[0];
        /** @type {number} */
        b = 0 | data.a[1];
        /** @type {number} */
        var c = 0 | data.a[2];
        /** @type {number} */
        var h = 0 | data.a[3];
        /** @type {number} */
        var eh = 0 | data.a[4];
        /** @type {number} */
        var fh = 0 | data.a[5];
        /** @type {number} */
        var gh = 0 | data.a[6];
        /** @type {number} */
        controlsCount = 0 | data.a[7];
        /** @type {number} */
        l = 0;
        for (; 64 > l; l++) {
          /** @type {number} */
          var _warmer = ((a >>> 2 | a << 30) ^ (a >>> 13 | a << 19) ^ (a >>> 22 | a << 10)) + (a & b ^ a & c ^ b & c) | 0;
          /** @type {number} */
          index = (controlsCount = controlsCount + ((eh >>> 6 | eh << 26) ^ (eh >>> 11 | eh << 21) ^ (eh >>> 25 | eh << 7)) | 0) + ((index = (index = eh & fh ^ ~eh & gh) + (0 | descname[l]) | 0) + (0 | n[l]) | 0) | 0;
          /** @type {number} */
          controlsCount = gh;
          /** @type {number} */
          gh = fh;
          /** @type {number} */
          fh = eh;
          /** @type {number} */
          eh = h + index | 0;
          /** @type {number} */
          h = c;
          /** @type {number} */
          c = b;
          /** @type {number} */
          b = a;
          /** @type {number} */
          a = index + _warmer | 0;
        }
        /** @type {number} */
        data.a[0] = data.a[0] + a | 0;
        /** @type {number} */
        data.a[1] = data.a[1] + b | 0;
        /** @type {number} */
        data.a[2] = data.a[2] + c | 0;
        /** @type {number} */
        data.a[3] = data.a[3] + h | 0;
        /** @type {number} */
        data.a[4] = data.a[4] + eh | 0;
        /** @type {number} */
        data.a[5] = data.a[5] + fh | 0;
        /** @type {number} */
        data.a[6] = data.a[6] + gh | 0;
        /** @type {number} */
        data.a[7] = data.a[7] + controlsCount | 0;
      }
      /**
       * @param {!Object} data
       * @param {!Object} text
       * @param {number} n
       * @return {undefined}
       */
      function test(data, text, n) {
        if (void 0 === n) {
          n = text.length;
        }
        /** @type {number} */
        var i = 0;
        var b = data.c;
        if (isString(text)) {
          for (; i < n;) {
            data.f[b++] = text.charCodeAt(i++);
            if (b == data.b) {
              fmt(data);
              /** @type {number} */
              b = 0;
            }
          }
        } else {
          if (!isArray(text)) {
            throw Error("message must be string or array");
          }
          for (; i < n;) {
            var c = text[i++];
            if (!("number" == typeof c && 0 <= c && 255 >= c && c == (0 | c))) {
              throw Error("message must be a byte array");
            }
            /** @type {number} */
            data.f[b++] = c;
            if (b == data.b) {
              fmt(data);
              /** @type {number} */
              b = 0;
            }
          }
        }
        data.c = b;
        data.g += n;
      }
      /**
       * @return {undefined}
       */
      function Image() {
        val.call(this, 8, ctx);
      }
      /**
       * @param {string} u
       * @param {number} p
       * @param {string} f
       * @param {string} x
       * @param {string} n
       * @return {undefined}
       */
      function r(u, p, f, x, n) {
        /** @type {string} */
        this.u = u;
        /** @type {number} */
        this.j = p;
        /** @type {string} */
        this.l = f;
        this.o = x || null;
        this.s = n || null;
        /** @type {string} */
        this.h = p + ":" + f;
        this.v = new function() {
          this.a = random();
        };
        this.g = new function(a) {
          /** @type {string} */
          this.a = a;
          this.b = random();
        }(this.h);
        /** @type {null} */
        this.f = null;
        /** @type {!Array} */
        this.b = [];
        /** @type {null} */
        this.a = this.c = null;
      }
      /**
       * @param {string} encoding
       * @return {?}
       */
      function fsReadFile(encoding) {
        return new Buffer("invalid-cordova-configuration", encoding);
      }
      /**
       * @param {!Array} node
       * @return {?}
       */
      function tick(node) {
        var map = new Image;
        test(map, node);
        /** @type {!Array} */
        node = [];
        /** @type {number} */
        var count = 8 * map.g;
        if (56 > map.c) {
          test(map, res, 56 - map.c);
        } else {
          test(map, res, map.b - (map.c - 56));
        }
        /** @type {number} */
        var i = 63;
        for (; 56 <= i; i--) {
          /** @type {number} */
          map.f[i] = 255 & count;
          /** @type {number} */
          count = count / 256;
        }
        fmt(map);
        /** @type {number} */
        i = count = 0;
        for (; i < map.j; i++) {
          /** @type {number} */
          var cbs = 24;
          for (; 0 <= cbs; cbs = cbs - 8) {
            /** @type {number} */
            node[count++] = map.a[i] >> cbs & 255;
          }
        }
        return function(value) {
          return defer(value, function(tmp) {
            return 1 < (tmp = tmp.toString(16)).length ? tmp : "0" + tmp;
          }).join("");
        }(node);
      }
      /**
       * @param {!Object} target
       * @param {?} property
       * @return {undefined}
       */
      function assign(target, property) {
        /** @type {number} */
        var i = 0;
        for (; i < target.b.length; i++) {
          try {
            target.b[i](property);
          } catch (t) {
          }
        }
      }
      /**
       * @param {!Object} o
       * @return {?}
       */
      function handle(o) {
        return o.f || (o.f = o.ea().then(function() {
          return new Promise(function(y) {
            o.va(function e(i) {
              return y(i), o.Ja(e), false;
            });
            (function(results) {
              /**
               * @param {!Object} track
               * @return {undefined}
               */
              function draw(track) {
                /** @type {boolean} */
                i = true;
                if (runner1) {
                  runner1.cancel();
                }
                draw(results).then(function(config) {
                  var c = b;
                  if (config && track && track.url) {
                    /** @type {null} */
                    var a = null;
                    if (-1 != (c = slice(track.url)).indexOf("/__/auth/callback")) {
                      a = (a = "object" == typeof(a = dispatch(isFinite(a = getValue(c), "firebaseError") || null)) ? getData(a) : null) ? new Node(config.b, config.c, null, null, a) : new Node(config.b, config.c, c, config.g);
                    }
                    c = a || b;
                  }
                  assign(results, c);
                });
              }
              var b = new Node("unknown", null, null, null, new Buffer("no-auth-event"));
              /** @type {boolean} */
              var i = false;
              var runner1 = validate(500).then(function() {
                return draw(results).then(function() {
                  if (!i) {
                    assign(results, b);
                  }
                });
              });
              /** @type {function(string): undefined} */
              var _current_callback = global.handleOpenURL;
              /**
               * @param {string} url
               * @return {undefined}
               */
              global.handleOpenURL = function(url) {
                if (0 == url.toLowerCase().indexOf(stringify("BuildInfo.packageName", global).toLowerCase() + "://") && draw({
                  url : url
                }), "function" == typeof _current_callback) {
                  try {
                    _current_callback(url);
                  } catch (t) {
                    console.error(t);
                  }
                }
              };
              if (!placeholder) {
                placeholder = new tester;
              }
              placeholder.subscribe(draw);
            })(o);
          });
        })), o.f;
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function draw(obj) {
        /** @type {null} */
        var i = null;
        return function(t) {
          return t.b.get(n, t.a).then(function(step) {
            return stop(step);
          });
        }(obj.g).then(function(e) {
          return i = e, insertText((e = obj.g).b, n, e.a);
        }).then(function() {
          return i;
        });
      }
      /**
       * @param {!Object} t
       * @return {?}
       */
      function currentStateIsSameWiki(t) {
        return insertText(t.b, candidate, t.a);
      }
      /**
       * @param {number} val
       * @param {number} doc
       * @param {string} handler
       * @return {undefined}
       */
      function result(val, doc, handler) {
        /** @type {number} */
        this.v = val;
        /** @type {number} */
        this.l = doc;
        /** @type {string} */
        this.u = handler;
        /** @type {!Array} */
        this.h = [];
        /** @type {boolean} */
        this.f = false;
        this.j = bind(this.o, this);
        this.c = new schedule;
        this.s = new Mix;
        this.g = new function(a) {
          /** @type {string} */
          this.a = a;
          this.b = random();
        }(this.l + ":" + this.u);
        this.b = {};
        this.b.unknown = this.c;
        this.b.signInViaRedirect = this.c;
        this.b.linkViaRedirect = this.c;
        this.b.reauthViaRedirect = this.c;
        this.b.signInViaPopup = this.s;
        this.b.linkViaPopup = this.s;
        this.b.reauthViaPopup = this.s;
        this.a = updateValues(this.v, this.l, this.u, project);
      }
      /**
       * @param {!Object} callback
       * @param {!Object} link
       * @param {!Function} pos
       * @param {!Object} marker
       * @return {?}
       */
      function updateValues(callback, link, pos, marker) {
        var p = config.SDK_VERSION || null;
        return resize() ? new r(callback, link, pos, p, marker) : new calc(callback, link, pos, p, marker);
      }
      /**
       * @param {!Object} item
       * @return {?}
       */
      function callbackFunction(item) {
        if (!item.f) {
          /** @type {boolean} */
          item.f = true;
          item.a.va(item.j);
        }
        var a = item.a;
        return item.a.ea().m(function(n) {
          throw item.a == a && item.reset(), n;
        });
      }
      /**
       * @param {!Object} request
       * @return {undefined}
       */
      function trigger(request) {
        if (request.a.Gb()) {
          callbackFunction(request).m(function(date) {
            var node = new Node("unknown", null, null, null, new Buffer("operation-not-supported-in-this-environment"));
            if (toString(date)) {
              request.o(node);
            }
          });
        }
        if (!request.a.Bb()) {
          isEmpty(request.c);
        }
      }
      /**
       * @param {!Object} t
       * @param {string} e
       * @param {string} i
       * @param {!Object} elem
       * @param {string} s
       * @param {boolean} m
       * @return {?}
       */
      function x(t, e, i, elem, s, m) {
        return t.a.xb(e, i, elem, function() {
          if (!t.f) {
            /** @type {boolean} */
            t.f = true;
            t.a.va(t.j);
          }
        }, function() {
          t.reset();
        }, s, m);
      }
      /**
       * @param {?} fmt
       * @return {?}
       */
      function toString(fmt) {
        return !(!fmt || "auth/cordova-not-ready" != fmt.code);
      }
      /**
       * @param {!Request} s
       * @param {string} type
       * @param {!Object} options
       * @return {?}
       */
      function chain(s, type, options) {
        /** @type {string} */
        var name = type + ":" + options;
        return events[name] || (events[name] = new result(s, type, options)), events[name];
      }
      /**
       * @return {undefined}
       */
      function schedule() {
        /** @type {null} */
        this.b = null;
        /** @type {!Array} */
        this.f = [];
        /** @type {!Array} */
        this.c = [];
        /** @type {null} */
        this.a = null;
        /** @type {boolean} */
        this.g = false;
      }
      /**
       * @param {!Object} obj
       * @return {undefined}
       */
      function isEmpty(obj) {
        if (!obj.g) {
          /** @type {boolean} */
          obj.g = true;
          print(obj, false, null, null);
        }
      }
      /**
       * @param {!Object} result
       * @param {!Object} data
       * @return {undefined}
       */
      function post(result, data) {
        if (result.b = function() {
          return resolve(data);
        }, result.f.length) {
          /** @type {number} */
          var i = 0;
          for (; i < result.f.length; i++) {
            result.f[i](data);
          }
        }
      }
      /**
       * @param {!Object} target
       * @param {boolean} type
       * @param {!Object} data
       * @param {string} json
       * @return {undefined}
       */
      function print(target, type, data, json) {
        if (type) {
          if (json) {
            (function(cell, json) {
              if (cell.b = function() {
                return cb(json);
              }, cell.c.length) {
                /** @type {number} */
                var i = 0;
                for (; i < cell.c.length; i++) {
                  cell.c[i](json);
                }
              }
            })(target, json);
          } else {
            post(target, data);
          }
        } else {
          post(target, {
            user : null
          });
        }
        /** @type {!Array} */
        target.f = [];
        /** @type {!Array} */
        target.c = [];
      }
      /**
       * @return {undefined}
       */
      function Mix() {
      }
      /**
       * @param {string} callback
       * @param {number} instance
       * @return {undefined}
       */
      function Server(callback, instance) {
        /** @type {number} */
        this.a = instance;
        debug(this, "verificationId", callback);
      }
      /**
       * @param {?} options
       * @param {string} value
       * @param {!Object} params
       * @param {boolean} url
       * @return {?}
       */
      function when(options, value, params, url) {
        return (new d(options)).Sa(value, params).then(function(interactiveServerStartCmd) {
          return new Server(interactiveServerStartCmd, url);
        });
      }
      /**
       * @param {number} key
       * @param {number} value
       * @param {string} count
       * @return {undefined}
       */
      function Range(key, value, count) {
        if (this.h = key, this.j = value, this.g = count, this.c = 3E4, this.f = 96E4, this.b = null, this.a = this.c, this.f < this.c) {
          throw Error("Proactive refresh lower bound greater than upper bound!");
        }
      }
      /**
       * @param {!Function} matrix
       * @return {undefined}
       */
      function _getMatrixAsString(matrix) {
        /** @type {!Function} */
        this.f = matrix;
        /** @type {null} */
        this.b = this.a = null;
        /** @type {number} */
        this.c = 0;
      }
      /**
       * @param {!Object} n
       * @param {!Object} a
       * @return {undefined}
       */
      function isUndefined(n, a) {
        var p = a[i];
        var key = a.refreshToken;
        a = compare(a.expiresIn);
        n.b = p;
        /** @type {!Object} */
        n.c = a;
        n.a = key;
      }
      /**
       * @param {?} b
       * @return {?}
       */
      function compare(b) {
        return min() + 1E3 * parseInt(b, 10);
      }
      /**
       * @param {!Object} data
       * @param {!Object} fn
       * @return {?}
       */
      function getToken(data, fn) {
        return function(c, data) {
          return new Promise(function(f_callBack, callback) {
            if ("refresh_token" == data.grant_type && data.refresh_token || "authorization_code" == data.grant_type && data.code) {
              install(c, c.j + "?key=" + encodeURIComponent(c.b), function(res) {
                if (res) {
                  if (res.error) {
                    callback(errorHandler(res));
                  } else {
                    if (res.access_token && res.refresh_token) {
                      f_callBack(res);
                    } else {
                      callback(new Buffer("internal-error"));
                    }
                  }
                } else {
                  callback(new Buffer("network-request-failed"));
                }
              }, "POST", model(data).toString(), c.f, c.l.get());
            } else {
              callback(new Buffer("internal-error"));
            }
          });
        }(data.f, fn).then(function(t) {
          return data.b = t.access_token, data.c = compare(t.expires_in), data.a = t.refresh_token, {
            accessToken : data.b,
            expirationTime : data.c,
            refreshToken : data.a
          };
        }).m(function(apiResponseError) {
          throw "auth/user-token-expired" == apiResponseError.code && (data.a = null), apiResponseError;
        });
      }
      /**
       * @param {string} a
       * @param {string} b
       * @return {undefined}
       */
      function Matrix(a, b) {
        this.a = a || null;
        this.b = b || null;
        done(this, {
          lastSignInTime : formatDate(b || null),
          creationTime : formatDate(a || null)
        });
      }
      /**
       * @param {?} params
       * @param {?} obj
       * @return {undefined}
       */
      function Element(params, obj) {
        var key;
        for (key in Event.call(this, params), obj) {
          this[key] = obj[key];
        }
      }
      /**
       * @param {!Object} options
       * @param {!Object} values
       * @param {number} dict
       * @return {undefined}
       */
      function update(options, values, dict) {
        /** @type {!Array} */
        this.D = [];
        this.G = options.apiKey;
        this.s = options.appName;
        this.B = options.authDomain || null;
        options = config.SDK_VERSION ? success(config.SDK_VERSION) : null;
        this.b = new Player(this.G, join(project), options);
        this.h = new _getMatrixAsString(this.b);
        emit(this, values[i]);
        isUndefined(this.h, values);
        debug(this, "refreshToken", this.h.a);
        createUser(this, dict || {});
        p.call(this);
        /** @type {boolean} */
        this.I = false;
        if (this.B && size()) {
          this.a = chain(this.B, this.G, this.s);
        }
        /** @type {!Array} */
        this.N = [];
        /** @type {null} */
        this.j = null;
        this.l = function(f) {
          return new Range(function() {
            return f.F(true);
          }, function(apiResponseError) {
            return !(!apiResponseError || "auth/network-request-failed" != apiResponseError.code);
          }, function() {
            /** @type {number} */
            var r = f.h.c - min() - 3E5;
            return 0 < r ? r : 0;
          });
        }(this);
        this.V = bind(this.Ga, this);
        var a = this;
        /** @type {null} */
        this.ia = null;
        /**
         * @param {!Object} n
         * @return {undefined}
         */
        this.sa = function(n) {
          a.na(n.g);
        };
        /** @type {null} */
        this.X = null;
        /** @type {!Array} */
        this.O = [];
        /**
         * @param {!Object} msg
         * @return {undefined}
         */
        this.ra = function(msg) {
          error(a, msg.c);
        };
        /** @type {null} */
        this.W = null;
      }
      /**
       * @param {!Object} data
       * @param {!Array} x
       * @return {undefined}
       */
      function logger(data, x) {
        if (data.X) {
          add(data.X, "languageCodeChanged", data.sa);
        }
        if (data.X = x) {
          map(x, "languageCodeChanged", data.sa);
        }
      }
      /**
       * @param {!Object} options
       * @param {boolean} f
       * @return {undefined}
       */
      function error(options, f) {
        /** @type {boolean} */
        options.O = f;
        reply(options.b, config.SDK_VERSION ? success(config.SDK_VERSION, options.O) : null);
      }
      /**
       * @param {!Object} node
       * @param {number} data
       * @return {undefined}
       */
      function store(node, data) {
        if (node.W) {
          add(node.W, "frameworkChanged", node.ra);
        }
        if (node.W = data) {
          map(data, "frameworkChanged", node.ra);
        }
      }
      /**
       * @param {!Object} options
       * @return {?}
       */
      function use(options) {
        try {
          return config.app(options.s).auth();
        } catch (e) {
          throw new Buffer("internal-error", "No firebase.auth.Auth instance is available for the Firebase App '" + options.s + "'!");
        }
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function processTemplate(data) {
        if (!(data.o || data.l.b)) {
          data.l.start();
          add(data, "tokenChanged", data.V);
          map(data, "tokenChanged", data.V);
        }
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function openModal(data) {
        add(data, "tokenChanged", data.V);
        data.l.stop();
      }
      /**
       * @param {!Object} options
       * @param {string} data
       * @return {undefined}
       */
      function emit(options, data) {
        /** @type {string} */
        options.qa = data;
        debug(options, "_lat", data);
      }
      /**
       * @param {!Object} value
       * @return {?}
       */
      function list(value) {
        /** @type {!Array} */
        var e = [];
        /** @type {number} */
        var name = 0;
        for (; name < value.N.length; name++) {
          e.push(value.N[name](value));
        }
        return function(keys) {
          return new Promise(function(cb) {
            /** @type {number} */
            var keysLeft = keys.length;
            /** @type {!Array} */
            var results = [];
            if (keysLeft) {
              /**
               * @param {undefined} index
               * @param {boolean} fulfilled
               * @param {string} result
               * @return {undefined}
               */
              var next = function(index, fulfilled, result) {
                keysLeft--;
                /** @type {({Zb: boolean, reason: ?}|{Zb: boolean, value: ?})} */
                results[index] = fulfilled ? {
                  Zb : true,
                  value : result
                } : {
                  Zb : false,
                  reason : result
                };
                if (0 == keysLeft) {
                  cb(results);
                }
              };
              /** @type {number} */
              var i = 0;
              for (; i < keys.length; i++) {
                compose(keys[i], listen(next, i, true), listen(next, i, false));
              }
            } else {
              cb(results);
            }
          });
        }(e).then(function() {
          return value;
        });
      }
      /**
       * @param {string} args
       * @return {undefined}
       */
      function getTitle(args) {
        if (args.a && !args.I) {
          /** @type {boolean} */
          args.I = true;
          args.a.subscribe(args);
        }
      }
      /**
       * @param {!Function} request
       * @param {!Object} user
       * @return {undefined}
       */
      function createUser(request, user) {
        done(request, {
          uid : user.uid,
          displayName : user.displayName || null,
          photoURL : user.photoURL || null,
          email : user.email || null,
          emailVerified : user.emailVerified || false,
          phoneNumber : user.phoneNumber || null,
          isAnonymous : user.isAnonymous || false,
          metadata : new Matrix(user.createdAt, user.lastLoginAt),
          providerData : []
        });
      }
      /**
       * @return {undefined}
       */
      function id() {
      }
      /**
       * @param {!Object} a
       * @return {?}
       */
      function reject(a) {
        return resolve().then(function() {
          if (a.o) {
            throw new Buffer("app-deleted");
          }
        });
      }
      /**
       * @param {!Object} obj
       * @return {?}
       */
      function getPath(obj) {
        return defer(obj.providerData, function(provider) {
          return provider.providerId;
        });
      }
      /**
       * @param {!Object} obj
       * @param {!Object} item
       * @return {undefined}
       */
      function includes(obj, item) {
        if (item) {
          contains(obj, item.providerId);
          obj.providerData.push(item);
        }
      }
      /**
       * @param {!Object} obj
       * @param {?} type
       * @return {undefined}
       */
      function contains(obj, type) {
        s(obj.providerData, function(event) {
          return event.providerId == type;
        });
      }
      /**
       * @param {!Object} target
       * @param {string} value
       * @param {boolean} n
       * @return {undefined}
       */
      function insert(target, value, n) {
        if (("uid" != value || n) && target.hasOwnProperty(value)) {
          debug(target, value, n);
        }
      }
      /**
       * @param {!Object} data
       * @param {!Object} user
       * @return {undefined}
       */
      function create(data, user) {
        if (data != user) {
          done(data, {
            uid : user.uid,
            displayName : user.displayName,
            photoURL : user.photoURL,
            email : user.email,
            emailVerified : user.emailVerified,
            phoneNumber : user.phoneNumber,
            isAnonymous : user.isAnonymous,
            providerData : []
          });
          if (user.metadata) {
            debug(data, "metadata", function(m) {
              return new Matrix(m.a, m.b);
            }(user.metadata));
          } else {
            debug(data, "metadata", new Matrix);
          }
          equal(user.providerData, function(type) {
            includes(data, type);
          });
          data.h = user.h;
          debug(data, "refreshToken", data.h.a);
        }
      }
      /**
       * @param {!Object} t
       * @return {?}
       */
      function stat(t) {
        return t.F().then(function(cache) {
          var cs = t.isAnonymous;
          return function(t, handler) {
            return callback(t.b, response, {
              idToken : handler
            }).then(bind(t.uc, t));
          }(t, cache).then(function() {
            return cs || insert(t, "isAnonymous", false), cache;
          });
        });
      }
      /**
       * @param {!Object} element
       * @param {!Object} selector
       * @return {undefined}
       */
      function clone(element, selector) {
        if (selector[i] && element.qa != selector[i]) {
          isUndefined(element.h, selector);
          element.dispatchEvent(new Element("tokenChanged"));
          emit(element, selector[i]);
          insert(element, "refreshToken", element.h.a);
        }
      }
      /**
       * @param {!Object} obj
       * @param {string} key
       * @return {?}
       */
      function put(obj, key) {
        return stat(obj).then(function() {
          if (push(getPath(obj), key)) {
            return list(obj).then(function() {
              throw new Buffer("provider-already-linked");
            });
          }
        });
      }
      /**
       * @param {string} obj
       * @param {string} val
       * @param {string} o
       * @return {?}
       */
      function toArray(obj, val, o) {
        return err({
          user : obj,
          credential : indexOf(val),
          additionalUserInfo : val = pick(val),
          operationType : o
        });
      }
      /**
       * @param {!Object} object
       * @param {!Object} parameters
       * @return {?}
       */
      function all(object, parameters) {
        return clone(object, parameters), object.reload().then(function() {
          return object;
        });
      }
      /**
       * @param {!Object} data
       * @param {string} i
       * @param {!Object} key
       * @param {!Function} e
       * @param {boolean} c
       * @return {?}
       */
      function remove(data, i, key, e, c) {
        if (!size()) {
          return cb(new Buffer("operation-not-supported-in-this-environment"));
        }
        if (data.j && !c) {
          return cb(data.j);
        }
        var html = info(key.providerId);
        var t = removeItem(data.uid + ":::");
        /** @type {null} */
        var value = null;
        if ((!query() || click()) && data.B && key.isOAuthProvider) {
          value = format(data.B, data.G, data.s, i, key, null, t, config.SDK_VERSION || null);
        }
        var params = render(value, html && html.Aa, html && html.za);
        return e = e().then(function() {
          if (parseModel(data), !c) {
            return data.F().then(function() {
            });
          }
        }).then(function() {
          return x(data.a, params, i, key, t, !!value);
        }).then(function() {
          return new Promise(function(from, elem) {
            data.ga(i, null, new Buffer("cancelled-popup-request"), data.g || null);
            /** @type {!Function} */
            data.f = from;
            /** @type {number} */
            data.v = elem;
            data.g = t;
            data.c = data.a.Da(data, i, params, t);
          });
        }).then(function(e) {
          return params && isObject(params), e ? err(e) : null;
        }).m(function(canCreateDiscussions) {
          throw params && isObject(params), canCreateDiscussions;
        }), call(data, e, c);
      }
      /**
       * @param {!Object} data
       * @param {string} p
       * @param {!Object} v
       * @param {!Function} t
       * @param {boolean} err
       * @return {?}
       */
      function set(data, p, v, t, err) {
        if (!size()) {
          return cb(new Buffer("operation-not-supported-in-this-environment"));
        }
        if (data.j && !err) {
          return cb(data.j);
        }
        /** @type {null} */
        var lastTrackInfoUrl = null;
        var value = removeItem(data.uid + ":::");
        return t = t().then(function() {
          if (parseModel(data), !err) {
            return data.F().then(function() {
            });
          }
        }).then(function() {
          return data.$ = value, list(data);
        }).then(function(msg) {
          return data.fa && (msg = (msg = data.fa).b.set(foo, data.C(), msg.a)), msg;
        }).then(function() {
          return data.a.Ba(p, v, value);
        }).m(function(trackInfoUrl) {
          if (lastTrackInfoUrl = trackInfoUrl, data.fa) {
            return text(data.fa);
          }
          throw lastTrackInfoUrl;
        }).then(function() {
          if (lastTrackInfoUrl) {
            throw lastTrackInfoUrl;
          }
        }), call(data, t, err);
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function parseModel(data) {
        if (!data.a || !data.I) {
          if (data.a && !data.I) {
            throw new Buffer("internal-error");
          }
          throw new Buffer("auth-domain-config-required");
        }
      }
      /**
       * @param {!Object} data
       * @param {string} e
       * @param {boolean} options
       * @return {?}
       */
      function call(data, e, options) {
        var query = function(t, e, res) {
          return t.j && !res ? (e.cancel(), cb(t.j)) : e.m(function(j) {
            throw !j || "auth/user-disabled" != j.code && "auth/user-token-expired" != j.code || (t.j || t.dispatchEvent(new Element("userInvalidated")), t.j = j), j;
          });
        }(data, e, options);
        return data.D.push(query), query.ha(function() {
          lookup(data.D, query);
        }), query;
      }
      /**
       * @param {!Object} options
       * @return {?}
       */
      function configure(options) {
        if (!options.apiKey) {
          return null;
        }
        var config = {
          apiKey : options.apiKey,
          authDomain : options.authDomain,
          appName : options.appName
        };
        var user = {};
        if (!(options.stsTokenManager && options.stsTokenManager.accessToken && options.stsTokenManager.expirationTime)) {
          return null;
        }
        user[i] = options.stsTokenManager.accessToken;
        user.refreshToken = options.stsTokenManager.refreshToken || null;
        /** @type {number} */
        user.expiresIn = (options.stsTokenManager.expirationTime - min()) / 1E3;
        var b = new update(config, user, options);
        return options.providerData && equal(options.providerData, function(path) {
          if (path) {
            includes(b, err(path));
          }
        }), options.redirectEventId && (b.$ = options.redirectEventId), b;
      }
      /**
       * @param {!Object} t
       * @return {?}
       */
      function text(t) {
        return insertText(t.b, foo, t.a);
      }
      /**
       * @param {number} a
       * @return {undefined}
       */
      function connect(a) {
        /** @type {number} */
        this.a = a;
        this.b = random();
        /** @type {null} */
        this.c = null;
        this.f = function(b) {
          var a = parseFloat("local");
          var o = parseFloat("session");
          var m = parseFloat("none");
          return function(a, o, i) {
            var e = trim(o, i);
            var input = jQuery(a, o.w);
            return a.get(o, i).then(function(timeline) {
              /** @type {null} */
              var s = null;
              try {
                s = dispatch(global.localStorage.getItem(e));
              } catch (t) {
              }
              if (s && !timeline) {
                return global.localStorage.removeItem(e), a.set(o, s, i);
              }
              if (s && timeline && "localStorage" != input.type) {
                global.localStorage.removeItem(e);
              }
            });
          }(b.b, a, b.a).then(function() {
            return b.b.get(o, b.a);
          }).then(function(inclSelf) {
            return inclSelf ? o : b.b.get(m, b.a).then(function(isBonds) {
              return isBonds ? m : b.b.get(a, b.a).then(function(noindent) {
                return noindent ? a : b.b.get(args, b.a).then(function(f) {
                  return f ? parseFloat(f) : a;
                });
              });
            });
          }).then(function(info) {
            return b.c = info, u(b, info.w);
          }).m(function() {
            if (!b.c) {
              b.c = a;
            }
          });
        }(this);
        this.b.addListener(parseFloat("local"), this.a, bind(this.g, this));
      }
      /**
       * @param {!Object} t
       * @param {string} c
       * @return {?}
       */
      function u(t, c) {
        var p;
        /** @type {!Array} */
        var a = [];
        for (p in tmp) {
          if (tmp[p] !== c) {
            a.push(insertText(t.b, parseFloat(tmp[p]), t.a));
          }
        }
        return a.push(insertText(t.b, args, t.a)), function(keys) {
          return new Promise(function($, filterStack) {
            /** @type {number} */
            var keysLeft = keys.length;
            /** @type {!Array} */
            var r = [];
            if (keysLeft) {
              /**
               * @param {undefined} f
               * @param {?} v
               * @return {undefined}
               */
              var mousemove = function(f, v) {
                keysLeft--;
                r[f] = v;
                if (0 == keysLeft) {
                  $(r);
                }
              };
              /**
               * @param {?} error
               * @return {undefined}
               */
              var stitch = function(error) {
                filterStack(error);
              };
              /** @type {number} */
              var i = 0;
              for (; i < keys.length; i++) {
                compose(keys[i], listen(mousemove, i), stitch);
              }
            } else {
              $(r);
            }
          });
        }(a);
      }
      /**
       * @param {string} value
       * @return {?}
       */
      function parseFloat(value) {
        return {
          name : "authUser",
          w : value
        };
      }
      /**
       * @param {!Object} e
       * @param {!Object} x
       * @return {?}
       */
      function C(e, x) {
        return encrypt(e, function() {
          return e.b.set(e.c, x.C(), e.a);
        });
      }
      /**
       * @param {!Object} t
       * @return {?}
       */
      function format_time(t) {
        return encrypt(t, function() {
          return insertText(t.b, t.c, t.a);
        });
      }
      /**
       * @param {!Object} self
       * @param {string} data
       * @return {?}
       */
      function decrypt(self, data) {
        return encrypt(self, function() {
          return self.b.get(self.c, self.a).then(function(options) {
            return options && data && (options.authDomain = data), configure(options || {});
          });
        });
      }
      /**
       * @param {!Object} obj
       * @param {!Function} options
       * @return {?}
       */
      function encrypt(obj, options) {
        return obj.f = obj.f.then(options, options), obj.f;
      }
      /**
       * @param {string} y
       * @return {undefined}
       */
      function init(y) {
        if (this.l = false, debug(this, "app", y), !build(this).options || !build(this).options.apiKey) {
          throw new Buffer("invalid-api-key");
        }
        y = config.SDK_VERSION ? success(config.SDK_VERSION) : null;
        this.b = new Player(build(this).options && build(this).options.apiKey, join(project), y);
        /** @type {!Array} */
        this.N = [];
        /** @type {!Array} */
        this.o = [];
        /** @type {!Array} */
        this.I = [];
        this.Ob = config.INTERNAL.createSubscribe(bind(this.ic, this));
        this.O = void 0;
        this.Pb = config.INTERNAL.createSubscribe(bind(this.jc, this));
        select(this, null);
        this.h = new connect(build(this).options.apiKey + ":" + build(this).name);
        this.G = new function(a) {
          /** @type {string} */
          this.a = a;
          this.b = random();
        }(build(this).options.apiKey + ":" + build(this).name);
        this.V = request(this, function(data) {
          var ecRndB = build(data).options.authDomain;
          var get = function(data) {
            var get = function(t, maskPos) {
              return t.b.get(foo, t.a).then(function(result) {
                return result && maskPos && (result.authDomain = maskPos), configure(result || {});
              });
            }(data.G, build(data).options.authDomain).then(function(value) {
              return (data.B = value) && (value.fa = data.G), text(data.G);
            });
            return request(data, get);
          }(data).then(function() {
            return decrypt(data.h, ecRndB);
          }).then(function(a) {
            return a ? (a.fa = data.G, data.B && (data.B.$ || null) == (a.$ || null) ? a : a.reload().then(function() {
              return C(data.h, a).then(function() {
                return a;
              });
            }).m(function(apiResponseError) {
              return "auth/network-request-failed" == apiResponseError.code ? a : format_time(data.h);
            })) : null;
          }).then(function(from) {
            select(data, from || null);
          });
          return request(data, get);
        }(this));
        this.j = request(this, function(c) {
          return c.V.then(function() {
            return c.da();
          }).m(function() {
          }).then(function() {
            if (!c.l) {
              return c.ia();
            }
          }).m(function() {
          }).then(function() {
            if (!c.l) {
              /** @type {boolean} */
              c.X = true;
              var t = c.h;
              t.b.addListener(parseFloat("local"), t.a, c.ia);
            }
          });
        }(this));
        /** @type {boolean} */
        this.X = false;
        this.ia = bind(this.Ic, this);
        this.Ga = bind(this.ka, this);
        this.qa = bind(this.Yb, this);
        this.ra = bind(this.gc, this);
        this.sa = bind(this.hc, this);
        (function(options) {
          var data = build(options).options.authDomain;
          var callback = build(options).options.apiKey;
          if (data && size()) {
            options.Nb = options.V.then(function() {
              if (!options.l) {
                if (options.a = chain(data, callback, build(options).name), options.a.subscribe(options), $(options) && getTitle($(options)), options.B) {
                  getTitle(options.B);
                  var data = options.B;
                  data.na(options.ca());
                  logger(data, options);
                  error(data = options.B, options.D);
                  store(data, options);
                  /** @type {null} */
                  options.B = null;
                }
                return options.a;
              }
            });
          }
        })(this);
        this.INTERNAL = {};
        this.INTERNAL.delete = bind(this.delete, this);
        this.INTERNAL.logFramework = bind(this.qc, this);
        /** @type {number} */
        this.s = 0;
        p.call(this);
        (function(proto) {
          Object.defineProperty(proto, "lc", {
            get : function() {
              return this.ca();
            },
            set : function(a) {
              this.na(a);
            },
            enumerable : false
          });
          /** @type {null} */
          proto.W = null;
        })(this);
        /** @type {!Array} */
        this.D = [];
      }
      /**
       * @param {string} identity
       * @return {undefined}
       */
      function _Event(identity) {
        Event.call(this, "languageCodeChanged");
        /** @type {string} */
        this.g = identity;
      }
      /**
       * @param {number} type
       * @return {undefined}
       */
      function ProgressEvent(type) {
        Event.call(this, "frameworkChanged");
        /** @type {number} */
        this.c = type;
      }
      /**
       * @param {?} hot
       * @return {?}
       */
      function saveFile(hot) {
        return hot.Nb || cb(new Buffer("auth-domain-config-required"));
      }
      /**
       * @param {!Object} node
       * @param {boolean} uri
       * @return {?}
       */
      function generate(node, uri) {
        var result = {};
        return result.apiKey = build(node).options.apiKey, result.authDomain = build(node).options.authDomain, result.appName = build(node).name, node.V.then(function() {
          return function(dataBuffer, className, n, err) {
            var data = new update(dataBuffer, className);
            return n && (data.fa = n), err && error(data, err), data.reload().then(function() {
              return data;
            });
          }(result, uri, node.G, node.Ka());
        }).then(function(data) {
          return $(node) && data.uid == $(node).uid ? (create($(node), data), node.ka(data)) : (select(node, data), getTitle(data), node.ka(data));
        }).then(function() {
          formatResult(node);
        });
      }
      /**
       * @param {!Object} options
       * @param {!Object} data
       * @return {undefined}
       */
      function select(options, data) {
        if ($(options)) {
          (function(unit, all) {
            s(unit.N, function(letter) {
              return letter == all;
            });
          })($(options), options.Ga);
          add($(options), "tokenChanged", options.qa);
          add($(options), "userDeleted", options.ra);
          add($(options), "userInvalidated", options.sa);
          openModal($(options));
        }
        if (data) {
          data.N.push(options.Ga);
          map(data, "tokenChanged", options.qa);
          map(data, "userDeleted", options.ra);
          map(data, "userInvalidated", options.sa);
          if (0 < options.s) {
            processTemplate(data);
          }
        }
        debug(options, "currentUser", data);
        if (data) {
          data.na(options.ca());
          logger(data, options);
          error(data, options.D);
          store(data, options);
        }
      }
      /**
       * @param {!Object} data
       * @param {?} e
       * @return {?}
       */
      function find(data, e) {
        /** @type {null} */
        var ref = null;
        /** @type {null} */
        var name = null;
        return request(data, e.then(function(val) {
          return ref = indexOf(val), name = pick(val), generate(data, val);
        }).then(function() {
          return err({
            user : $(data),
            credential : ref,
            additionalUserInfo : name,
            operationType : "signIn"
          });
        }));
      }
      /**
       * @param {!Object} t
       * @return {?}
       */
      function build(t) {
        return t.app;
      }
      /**
       * @param {!Object} name
       * @return {?}
       */
      function $(name) {
        return name.currentUser;
      }
      /**
       * @param {!Object} num
       * @return {?}
       */
      function lz(num) {
        return $(num) && $(num)._lat || null;
      }
      /**
       * @param {!Object} data
       * @return {undefined}
       */
      function formatResult(data) {
        if (data.X) {
          /** @type {number} */
          var i = 0;
          for (; i < data.o.length; i++) {
            if (data.o[i]) {
              data.o[i](lz(data));
            }
          }
          if (data.O !== data.getUid() && data.I.length) {
            data.O = data.getUid();
            /** @type {number} */
            i = 0;
            for (; i < data.I.length; i++) {
              if (data.I[i]) {
                data.I[i](lz(data));
              }
            }
          }
        }
      }
      /**
       * @param {!Object} data
       * @param {string} message
       * @return {?}
       */
      function request(data, message) {
        return data.N.push(message), message.ha(function() {
          lookup(data.N, message);
        }), message;
      }
      /**
       * @param {string} options
       * @param {!Object} document
       * @param {string} config
       * @param {!Object} src
       * @param {number} dataType
       * @param {string} data
       * @return {undefined}
       */
      function run(options, document, config, src, dataType, data) {
        if (debug(this, "type", "recaptcha"), this.b = this.c = null, this.o = false, this.l = document, this.a = config || {
          theme : "light",
          type : "image"
        }, this.g = [], this.a[index]) {
          throw new Buffer("argument-error", "sitekey should not be provided for reCAPTCHA as one is automatically provisioned for the current project.");
        }
        if (this.h = "invisible" === this.a[SIZEFIELD], !global.document) {
          throw new Buffer("operation-not-supported-in-this-environment", "RecaptchaVerifier is only supported in a browser HTTP/HTTPS environment with DOM support.");
        }
        if (!removeEventListener(document) || !this.h && removeEventListener(document).hasChildNodes()) {
          throw new Buffer("argument-error", "reCAPTCHA container is either not found or already contains inner elements!");
        }
        this.u = new Player(options, data || null, dataType || null);
        this.s = src || function() {
          return null;
        };
        var style = this;
        /** @type {!Array} */
        this.j = [];
        var value = this.a[propertyName];
        /**
         * @param {!Object} name
         * @return {undefined}
         */
        this.a[propertyName] = function(name) {
          if (setProperty(style, name), "function" == typeof value) {
            value(name);
          } else {
            if ("string" == typeof value) {
              var result = stringify(value, global);
              if ("function" == typeof result) {
                result(name);
              }
            }
          }
        };
        var c = this.a[id_item];
        /**
         * @return {undefined}
         */
        this.a[id_item] = function() {
          if (setProperty(style, null), "function" == typeof c) {
            c();
          } else {
            if ("string" == typeof c) {
              var result = stringify(c, global);
              if ("function" == typeof result) {
                result();
              }
            }
          }
        };
      }
      /**
       * @param {!Object} component
       * @param {!Object} req
       * @return {undefined}
       */
      function setProperty(component, req) {
        /** @type {number} */
        var ii = 0;
        for (; ii < component.j.length; ii++) {
          try {
            component.j[ii](req);
          } catch (t) {
          }
        }
      }
      /**
       * @param {!Object} options
       * @param {string} data
       * @return {?}
       */
      function verify(options, data) {
        return options.g.push(data), data.ha(function() {
          lookup(options.g, data);
        }), data;
      }
      /**
       * @param {!Object} table
       * @return {undefined}
       */
      function save(table) {
        if (table.o) {
          throw new Buffer("internal-error", "RecaptchaVerifier instance has been destroyed.");
        }
      }
      /**
       * @return {?}
       */
      function splice() {
        return Mc || (Mc = new function() {
          /** @type {number} */
          this.b = global.grecaptcha ? 1 / 0 : 0;
          /** @type {null} */
          this.c = null;
          /** @type {string} */
          this.a = "__rcb" + Math.floor(1E6 * Math.random()).toString();
        }), Mc;
      }
      /**
       * @param {?} data
       * @param {?} options
       * @param {string} fn
       * @return {undefined}
       */
      function auth(data, options, fn) {
        try {
          this.f = fn || config.app();
        } catch (t) {
          throw new Buffer("argument-error", "No firebase.app.App instance is currently initialized.");
        }
        if (!this.f.options || !this.f.options.apiKey) {
          throw new Buffer("invalid-api-key");
        }
        fn = this.f.options.apiKey;
        var constants = this;
        /** @type {null} */
        var c = null;
        try {
          c = this.f.auth().Ka();
        } catch (t) {
        }
        c = config.SDK_VERSION ? success(config.SDK_VERSION, c) : null;
        run.call(this, fn, data, options, function() {
          try {
            var data = constants.f.auth().ca();
          } catch (e) {
            /** @type {null} */
            data = null;
          }
          return data;
        }, c, join(project));
      }
      /**
       * @param {string} type
       * @param {!Object} obj
       * @param {string} path
       * @param {boolean} name
       * @return {undefined}
       */
      function delegate(type, obj, path, name) {
        t: {
          /** @type {!Array<?>} */
          path = Array.prototype.slice.call(path);
          /** @type {number} */
          var index = 0;
          /** @type {boolean} */
          var size = false;
          /** @type {number} */
          var i = 0;
          for (; i < obj.length; i++) {
            if (obj[i].optional) {
              /** @type {boolean} */
              size = true;
            } else {
              if (size) {
                throw new Buffer("internal-error", "Argument validator encountered a required argument after an optional argument.");
              }
              index++;
            }
          }
          if (size = obj.length, path.length < index || size < path.length) {
            /** @type {string} */
            name = "Expected " + (index == size ? 1 == index ? "1 argument" : index + " arguments" : index + "-" + size + " arguments") + " but got " + path.length + ".";
          } else {
            /** @type {number} */
            index = 0;
            for (; index < path.length; index++) {
              if (size = obj[index].optional && void 0 === path[index], !obj[index].M(path[index]) && !size) {
                if (obj = obj[index], 0 > index || index >= shapes.length) {
                  throw new Buffer("internal-error", "Argument validator received an unsupported number of arguments.");
                }
                /** @type {string} */
                path = shapes[index];
                /** @type {string} */
                name = (name ? "" : path + " argument ") + (obj.name ? '"' + obj.name + '" ' : "") + "must be " + obj.K + ".";
                break t;
              }
            }
            /** @type {null} */
            name = null;
          }
        }
        if (name) {
          throw new Buffer("argument-error", type + " failed: " + name);
        }
      }
      /**
       * @param {string} str
       * @param {boolean} raw
       * @return {?}
       */
      function wrap(str, raw) {
        return {
          name : str || "",
          K : "a valid string",
          optional : !!raw,
          M : isString
        };
      }
      /**
       * @return {?}
       */
      function castTemplate() {
        return {
          name : "opt_forceRefresh",
          K : "a boolean",
          optional : true,
          M : translate
        };
      }
      /**
       * @param {string} data
       * @param {boolean} is_array_item
       * @return {?}
       */
      function deserialize(data, is_array_item) {
        return {
          name : data || "",
          K : "a valid object",
          optional : !!is_array_item,
          M : isFunction
        };
      }
      /**
       * @param {string} data
       * @param {boolean} falsy
       * @return {?}
       */
      function createFilter(data, falsy) {
        return {
          name : data || "",
          K : "a function",
          optional : !!falsy,
          M : isNaN
        };
      }
      /**
       * @param {string} name
       * @param {boolean} index
       * @return {?}
       */
      function _pretty_print_properties(name, index) {
        return {
          name : name || "",
          K : "null",
          optional : !!index,
          M : processRelativeTime
        };
      }
      /**
       * @param {string} name
       * @return {?}
       */
      function setData(name) {
        return {
          name : name ? name + "Credential" : "credential",
          K : name ? "a valid " + name + " credential" : "a valid credential",
          optional : false,
          M : function(item) {
            if (!item) {
              return false;
            }
            /** @type {boolean} */
            var n = !name || item.providerId === name;
            return !(!item.xa || !n);
          }
        };
      }
      /**
       * @return {?}
       */
      function getShortcuts() {
        return {
          name : "applicationVerifier",
          K : "an implementation of firebase.auth.ApplicationVerifier",
          optional : false,
          M : function(type) {
            return !!(type && isString(type.type) && isNaN(type.verify));
          }
        };
      }
      /**
       * @param {?} params
       * @param {?} options
       * @param {string} callback
       * @param {boolean} accessToken
       * @return {?}
       */
      function registerAccount(params, options, callback, accessToken) {
        return {
          name : callback || "",
          K : params.K + " or " + options.K,
          optional : !!accessToken,
          M : function(x) {
            return params.M(x) || options.M(x);
          }
        };
      }
      /**
       * @param {!Object} target
       * @param {!Object} data
       * @return {undefined}
       */
      function _extend(target, data) {
        var i;
        for (i in data) {
          var key = data[i].name;
          target[key] = merge(key, target[i], data[i].i);
        }
      }
      /**
       * @param {!Function} obj
       * @param {string} key
       * @param {!Function} name
       * @param {!Array} context
       * @return {undefined}
       */
      function recurse(obj, key, name, context) {
        obj[key] = merge(key, name, context);
      }
      /**
       * @param {string} el
       * @param {!Object} o
       * @param {!Object} value
       * @return {?}
       */
      function merge(el, o, value) {
        /**
         * @return {?}
         */
        function copy() {
          /** @type {!Array<?>} */
          var p = Array.prototype.slice.call(arguments);
          return delegate(textbox, value, p), o.apply(this, p);
        }
        if (!value) {
          return o;
        }
        var i;
        var textbox = function(diff) {
          return (diff = diff.split("."))[diff.length - 1];
        }(el);
        for (i in o) {
          copy[i] = o[i];
        }
        for (i in o.prototype) {
          copy.prototype[i] = o.prototype[i];
        }
        return copy;
      }
      var data;
      var config = definition.default;
      var Directory = Directory || {};
      var global = this;
      /** @type {string} */
      var eventIDBase = "closure_uid_" + (1e9 * Math.random() >>> 0);
      /** @type {number} */
      var eventIDCount = 0;
      /** @type {function(): number} */
      var min = Date.now || function() {
        return +new Date;
      };
      expect(Constructor, Error);
      /** @type {string} */
      Constructor.prototype.name = "CustomError";
      expect(Group, Constructor);
      /** @type {string} */
      Group.prototype.name = "AssertionError";
      /**
       * @return {?}
       */
      Matrix2D.prototype.get = function() {
        if (0 < this.b) {
          this.b--;
          var a = this.a;
          this.a = a.next;
          /** @type {null} */
          a.next = null;
        } else {
          a = this.c();
        }
        return a;
      };
      var _context = new Matrix2D(function() {
        return new Controller;
      }, function(applyViewModelsSpy) {
        applyViewModelsSpy.reset();
      });
      /**
       * @param {string} o
       * @param {!Object} a
       * @return {undefined}
       */
      Ebur128.prototype.add = function(o, a) {
        var value = _context.get();
        value.set(o, a);
        if (this.b) {
          this.b.next = value;
        } else {
          this.a = value;
        }
        this.b = value;
      };
      /**
       * @param {string} a
       * @param {string} b
       * @return {undefined}
       */
      Controller.prototype.set = function(a, b) {
        /** @type {string} */
        this.a = a;
        /** @type {string} */
        this.b = b;
        /** @type {null} */
        this.next = null;
      };
      /**
       * @return {undefined}
       */
      Controller.prototype.reset = function() {
        /** @type {null} */
        this.next = this.b = this.a = null;
      };
      /** @type {function(!Array, string): ?} */
      var substr = Array.prototype.indexOf ? function(objs, start) {
        return Array.prototype.indexOf.call(objs, start, void 0);
      } : function(array, value) {
        if (isString(array)) {
          return isString(value) && 1 == value.length ? array.indexOf(value, 0) : -1;
        }
        /** @type {number} */
        var i = 0;
        for (; i < array.length; i++) {
          if (i in array && array[i] === value) {
            return i;
          }
        }
        return -1;
      };
      /** @type {function(!Object, !Function, !Object): undefined} */
      var equal = Array.prototype.forEach ? function(o, event, n) {
        Array.prototype.forEach.call(o, event, n);
      } : function(arr, f, n) {
        var arrSize = arr.length;
        var arr2 = isString(arr) ? arr.split("") : arr;
        /** @type {number} */
        var i = 0;
        for (; i < arrSize; i++) {
          if (i in arr2) {
            f.call(n, arr2[i], i, arr);
          }
        }
      };
      /** @type {function(!Object, !Function): ?} */
      var defer = Array.prototype.map ? function(a, p1__3354_SHARP_) {
        return Array.prototype.map.call(a, p1__3354_SHARP_, void 0);
      } : function(obj, callback) {
        var length = obj.length;
        /** @type {!Array} */
        var results = Array(length);
        var el = isString(obj) ? obj.split("") : obj;
        /** @type {number} */
        var key = 0;
        for (; key < length; key++) {
          if (key in el) {
            results[key] = callback.call(void 0, el[key], key, obj);
          }
        }
        return results;
      };
      /** @type {function(?, !Function): ?} */
      var convertToRgbInt = Array.prototype.some ? function(gen34_options, p1__3354_SHARP_) {
        return Array.prototype.some.call(gen34_options, p1__3354_SHARP_, void 0);
      } : function(arr, f) {
        var arrSize = arr.length;
        var arr2 = isString(arr) ? arr.split("") : arr;
        /** @type {number} */
        var i = 0;
        for (; i < arrSize; i++) {
          if (i in arr2 && f.call(void 0, arr2[i], i, arr)) {
            return true;
          }
        }
        return false;
      };
      /** @type {function(string): ?} */
      var escapeHtml = String.prototype.trim ? function(commentToCheck) {
        return commentToCheck.trim();
      } : function(mapCommand) {
        return /^[\s\xa0]*([\s\S]*?)[\s\xa0]*$/.exec(mapCommand)[1];
      };
      var v;
      /** @type {!RegExp} */
      var refLinkIDRegex = /&/g;
      /** @type {!RegExp} */
      var spanOpenOrCloseRegExp = /</g;
      /** @type {!RegExp} */
      var regexSeparators = />/g;
      /** @type {!RegExp} */
      var json_escapable = /"/g;
      /** @type {!RegExp} */
      var ESCAPE = /'/g;
      /** @type {!RegExp} */
      var reA = /\x00/g;
      /** @type {!RegExp} */
      var regBigBrackets = /[\x00&<>"']/;
      t: {
        var nav = global.navigator;
        if (nav) {
          var ua = nav.userAgent;
          if (ua) {
            v = ua;
            break t;
          }
        }
        /** @type {string} */
        v = "";
      }
      var openScorm;
      var open;
      /** @type {!Array<string>} */
      var intFields = "constructor hasOwnProperty isPrototypeOf propertyIsEnumerable toLocaleString toString valueOf".split(" ");
      /** @type {boolean} */
      var dt = false;
      var clist = new Ebur128;
      /** @type {number} */
      var image = 0;
      /** @type {number} */
      var feature = 2;
      /** @type {number} */
      var object = 3;
      /**
       * @return {undefined}
       */
      RunHandlerTask.prototype.reset = function() {
        /** @type {null} */
        this.f = this.b = this.g = this.a = null;
        /** @type {boolean} */
        this.c = false;
      };
      var search = new Matrix2D(function() {
        return new RunHandlerTask;
      }, function(applyViewModelsSpy) {
        applyViewModelsSpy.reset();
      });
      /**
       * @param {!Function} e
       * @param {string} value
       * @param {!Object} cb
       * @return {?}
       */
      Promise.prototype.then = function(e, value, cb) {
        return html(this, isNaN(e) ? e : null, isNaN(value) ? value : null, cb);
      };
      defineProperty(Promise);
      /**
       * @param {!Function} callback
       * @param {!Function} options
       * @return {?}
       */
      (data = Promise.prototype).ha = function(callback, options) {
        return (callback = write(callback, callback, options)).c = true, forEach(this, callback), this;
      };
      /**
       * @param {!Function} date
       * @param {!Object} cb
       * @return {?}
       */
      data.m = function(date, cb) {
        return html(this, null, date, cb);
      };
      /**
       * @param {string} action
       * @return {undefined}
       */
      data.cancel = function(action) {
        if (this.a == image) {
          wrapper(function() {
            !function t(a, n) {
              if (a.a == image) {
                if (a.c) {
                  var b = a.c;
                  if (b.b) {
                    /** @type {number} */
                    var t0 = 0;
                    /** @type {null} */
                    var value = null;
                    /** @type {null} */
                    var t1 = null;
                    var t = b.b;
                    for (; t && (t.c || (t0++, t.a == a && (value = t), !(value && 1 < t0))); t = t.next) {
                      if (!value) {
                        t1 = t;
                      }
                    }
                    if (value) {
                      if (b.a == image && 1 == t0) {
                        t(b, n);
                      } else {
                        if (t1) {
                          if ((t0 = t1).next == b.f) {
                            b.f = t0;
                          }
                          t0.next = t0.next.next;
                        } else {
                          calcPoint(b);
                        }
                        e(b, value, object, n);
                      }
                    }
                  }
                  /** @type {null} */
                  a.c = null;
                } else {
                  handler(a, object, n);
                }
              }
            }(this, new Action(action));
          }, this);
        }
      };
      /**
       * @param {?} url
       * @return {undefined}
       */
      data.Kc = function(url) {
        /** @type {number} */
        this.a = image;
        handler(this, feature, url);
      };
      /**
       * @param {?} url
       * @return {undefined}
       */
      data.Lc = function(url) {
        /** @type {number} */
        this.a = image;
        handler(this, object, url);
      };
      /**
       * @return {undefined}
       */
      data.Ub = function() {
        var nbsp;
        for (; nbsp = calcPoint(this);) {
          e(this, nbsp, this.a, this.j);
        }
        /** @type {boolean} */
        this.h = false;
      };
      /** @type {function(?): undefined} */
      var onBodyKeyup = del;
      expect(Action, Constructor);
      /** @type {string} */
      Action.prototype.name = "cancel";
      /** @type {number} */
      var disconnected = 0;
      var nextIdLookup = {};
      /** @type {boolean} */
      method.prototype.pa = false;
      /**
       * @return {undefined}
       */
      method.prototype.ua = function() {
        if (this.oa) {
          for (; this.oa.length;) {
            this.oa.shift()();
          }
        }
      };
      /** @type {function(): undefined} */
      _getTypeFromWsdl[" "] = noop;
      var expected;
      var color;
      var startYNew = floor("Opera");
      var window = floor("Trident") || floor("MSIE");
      var base = floor("Edge");
      var current = base || window;
      var visible = floor("Gecko") && !(has(v.toLowerCase(), "webkit") && !floor("Edge")) && !(floor("Trident") || floor("MSIE")) && !floor("Edge");
      var elective = has(v.toLowerCase(), "webkit") && !floor("Edge");
      t: {
        /** @type {string} */
        var key = "";
        /** @type {(Array<string>|null|undefined)} */
        var node = (color = v, visible ? /rv:([^\);]+)(\)|;)/.exec(color) : base ? /Edge\/([\d\.]+)/.exec(color) : window ? /\b(?:MSIE|rv)[: ]([^\);]+)(\)|;)/.exec(color) : elective ? /WebKit\/(\S+)/.exec(color) : startYNew ? /(?:Version)[ \/]?(\S+)/.exec(color) : void 0);
        if (node && (key = node ? node[1] : ""), window) {
          var value = factory();
          if (null != value && value > parseFloat(key)) {
            /** @type {string} */
            expected = String(value);
            break t;
          }
        }
        /** @type {string} */
        expected = key;
      }
      var element;
      var descmain = {};
      var document = global.document;
      element = document && window ? factory() || ("CSS1Compat" == document.compatMode ? parseInt(expected, 10) : 5) : void 0;
      /** @type {function(T): T} */
      var getAlignItem = Object.freeze || function(partKeys) {
        return partKeys;
      };
      /** @type {boolean} */
      var W3C_MODEL = !window || 9 <= Number(element);
      var quiet = window && !apply("9");
      var ae = function() {
        if (!global.addEventListener || !Object.defineProperty) {
          return false;
        }
        /** @type {boolean} */
        var t = false;
        /** @type {!Object} */
        var options = Object.defineProperty({}, "passive", {
          get : function() {
            /** @type {boolean} */
            t = true;
          }
        });
        return global.addEventListener("test", noop, options), global.removeEventListener("test", noop, options), t;
      }();
      /**
       * @return {undefined}
       */
      Event.prototype.preventDefault = function() {
        /** @type {boolean} */
        this.Eb = false;
      };
      expect(start, Event);
      var MSPOINTER_TYPES = getAlignItem({
        2 : "touch",
        3 : "pen",
        4 : "mouse"
      });
      /**
       * @return {undefined}
       */
      start.prototype.preventDefault = function() {
        start.lb.preventDefault.call(this);
        var a = this.a;
        if (a.preventDefault) {
          a.preventDefault();
        } else {
          if (a.returnValue = false, quiet) {
            try {
              if (a.ctrlKey || 112 <= a.keyCode && 123 >= a.keyCode) {
                /** @type {number} */
                a.keyCode = -1;
              }
            } catch (t) {
            }
          }
        }
      };
      /**
       * @return {?}
       */
      start.prototype.f = function() {
        return this.a;
      };
      /** @type {string} */
      var key = "closure_listenable_" + (1E6 * Math.random() | 0);
      /** @type {number} */
      var nextKey = 0;
      /**
       * @param {string} value
       * @param {string} object
       * @param {boolean} hook
       * @param {string} obj
       * @param {!Array} options
       * @return {?}
       */
      Model.prototype.add = function(value, object, hook, obj, options) {
        var i = value.toString();
        if (!(value = this.a[i])) {
          /** @type {!Array} */
          value = this.a[i] = [];
          this.b++;
        }
        var key = iterator(value, object, obj, options);
        return -1 < key ? (object = value[key], hook || (object.Ha = false)) : ((object = new function(aListener, src, a, i, canCreateDiscussions) {
          /** @type {!Function} */
          this.listener = aListener;
          /** @type {null} */
          this.proxy = null;
          /** @type {string} */
          this.src = src;
          /** @type {!Object} */
          this.type = a;
          /** @type {boolean} */
          this.capture = !!i;
          this.La = canCreateDiscussions;
          /** @type {number} */
          this.key = ++nextKey;
          /** @type {boolean} */
          this.ma = this.Ha = false;
        }(object, this.src, i, !!obj, options)).Ha = hook, value.push(object)), object;
      };
      /** @type {string} */
      var part = "closure_lm_" + (1E6 * Math.random() | 0);
      var appenderMap = {};
      /** @type {string} */
      var CLIENT_TOP = "__closure_events_fn_" + (1E9 * Math.random() >>> 0);
      expect(p, method);
      /** @type {boolean} */
      p.prototype[key] = true;
      /**
       * @param {string} name
       * @param {!Object} options
       * @param {?} fn
       * @param {!Array} callback
       * @return {undefined}
       */
      p.prototype.addEventListener = function(name, options, fn, callback) {
        map(this, name, options, fn, callback);
      };
      /**
       * @param {string} type
       * @param {!Array} fn
       * @param {string} selector
       * @param {!Array} el
       * @return {undefined}
       */
      p.prototype.removeEventListener = function(type, fn, selector, el) {
        add(this, type, fn, selector, el);
      };
      /**
       * @param {!Object} event
       * @return {?}
       */
      p.prototype.dispatchEvent = function(event) {
        var data;
        var target = this.Ta;
        if (target) {
          /** @type {!Array} */
          data = [];
          for (; target; target = target.Ta) {
            data.push(target);
          }
        }
        target = this.Mb;
        var e = event.type || event;
        if (isString(event)) {
          event = new Event(event, target);
        } else {
          if (event instanceof Event) {
            event.target = event.target || target;
          } else {
            /** @type {!Object} */
            var result = event;
            log(event = new Event(e, target), result);
          }
        }
        if (result = true, data) {
          /** @type {number} */
          var i = data.length - 1;
          for (; 0 <= i; i--) {
            var value = event.b = data[i];
            result = invoke(value, e, true, event) && result;
          }
        }
        if (result = invoke(value = event.b = target, e, true, event) && result, result = invoke(value, e, false, event) && result, data) {
          /** @type {number} */
          i = 0;
          for (; i < data.length; i++) {
            result = invoke(value = event.b = data[i], e, false, event) && result;
          }
        }
        return result;
      };
      /**
       * @return {undefined}
       */
      p.prototype.ua = function() {
        if (p.lb.ua.call(this), this.u) {
          var attri;
          var gadgets = this.u;
          for (attri in gadgets.a) {
            var hlist = gadgets.a[attri];
            /** @type {number} */
            var i = 0;
            for (; i < hlist.length; i++) {
              abs(hlist[i]);
            }
            delete gadgets.a[attri];
            gadgets.b--;
          }
        }
        /** @type {null} */
        this.Ta = null;
      };
      /**
       * @return {?}
       */
      (data = Vector.prototype).S = function() {
        selectItem(this);
        /** @type {!Array} */
        var context = [];
        /** @type {number} */
        var i = 0;
        for (; i < this.a.length; i++) {
          context.push(this.b[this.a[i]]);
        }
        return context;
      };
      /**
       * @return {?}
       */
      data.U = function() {
        return selectItem(this), this.a.concat();
      };
      /**
       * @return {undefined}
       */
      data.clear = function() {
        this.b = {};
        /** @type {number} */
        this.c = this.a.length = 0;
      };
      /**
       * @param {string} name
       * @param {string} a
       * @return {?}
       */
      data.get = function(name, a) {
        return hasOwnProperty(this.b, name) ? this.b[name] : a;
      };
      /**
       * @param {string} name
       * @param {!Object} b
       * @return {undefined}
       */
      data.set = function(name, b) {
        if (!hasOwnProperty(this.b, name)) {
          this.c++;
          this.a.push(name);
        }
        /** @type {!Object} */
        this.b[name] = b;
      };
      /**
       * @param {!Function} f
       * @param {!Object} context
       * @return {undefined}
       */
      data.forEach = function(f, context) {
        var data = this.U();
        /** @type {number} */
        var a = 0;
        for (; a < data.length; a++) {
          var i = data[a];
          var e = this.get(i);
          f.call(context, e, i, this);
        }
      };
      /** @type {!RegExp} */
      var re2 = /^(?:([^:/?#.]+):)?(?:\/\/(?:([^/?#]*)@)?([^/#?]*?)(?::([0-9]+))?(?=[/#?]|$))?([^?#]+)?(?:\?([^#]*))?(?:#([\s\S]*))?$/;
      /**
       * @return {?}
       */
      RegExp.prototype.toString = function() {
        /** @type {!Array} */
        var test = [];
        var c = this.c;
        if (c) {
          test.push(traverse(c, schema, true), ":");
        }
        var r = this.b;
        return (r || "file" == c) && (test.push("//"), (c = this.l) && test.push(traverse(c, schema, true), "@"), test.push(encodeURIComponent(String(r)).replace(/%25([0-9a-fA-F]{2})/g, "%$1")), null != (r = this.j) && test.push(":", String(r))), (r = this.g) && (this.b && "/" != r.charAt(0) && test.push("/"), test.push(traverse(r, "/" == r.charAt(0) ? bufVal : url, true))), (r = this.a.toString()) && test.push("?", r), (r = this.h) && test.push("#", traverse(r, name)), test.join("");
      };
      /** @type {!RegExp} */
      var schema = /[#\/\?@]/g;
      /** @type {!RegExp} */
      var url = /[#\?:]/g;
      /** @type {!RegExp} */
      var bufVal = /[#\?]/g;
      /** @type {!RegExp} */
      var selector = /[#\?@]/g;
      /** @type {!RegExp} */
      var name = /#/g;
      /**
       * @param {string} a
       * @param {!Object} b
       * @return {?}
       */
      (data = A.prototype).add = function(a, b) {
        initialize(this);
        /** @type {null} */
        this.c = null;
        a = fn(this, a);
        var result = this.a.get(a);
        return result || this.a.set(a, result = []), result.push(b), this.b += 1, this;
      };
      /**
       * @return {undefined}
       */
      data.clear = function() {
        /** @type {null} */
        this.a = this.c = null;
        /** @type {number} */
        this.b = 0;
      };
      /**
       * @param {!Function} data
       * @param {!Object} obj
       * @return {undefined}
       */
      data.forEach = function(data, obj) {
        initialize(this);
        this.a.forEach(function(n, end) {
          equal(n, function(n) {
            data.call(obj, n, end, this);
          }, this);
        }, this);
      };
      /**
       * @return {?}
       */
      data.U = function() {
        initialize(this);
        var requiredNodes = this.a.S();
        var cutouts = this.a.U();
        /** @type {!Array} */
        var result = [];
        /** @type {number} */
        var i = 0;
        for (; i < cutouts.length; i++) {
          var r = requiredNodes[i];
          /** @type {number} */
          var o = 0;
          for (; o < r.length; o++) {
            result.push(cutouts[i]);
          }
        }
        return result;
      };
      /**
       * @param {!Object} item
       * @return {?}
       */
      data.S = function(item) {
        initialize(this);
        /** @type {!Array} */
        var fraction = [];
        if (isString(item)) {
          if (appendChild(this, item)) {
            fraction = F(fraction, this.a.get(fn(this, item)));
          }
        } else {
          item = this.a.S();
          /** @type {number} */
          var kItem = 0;
          for (; kItem < item.length; kItem++) {
            fraction = F(fraction, item[kItem]);
          }
        }
        return fraction;
      };
      /**
       * @param {string} obj
       * @param {!Object} name
       * @return {?}
       */
      data.set = function(obj, name) {
        return initialize(this), this.c = null, appendChild(this, obj = fn(this, obj)) && (this.b -= this.a.get(obj).length), this.a.set(obj, [name]), this.b += 1, this;
      };
      /**
       * @param {string} a
       * @param {string} itemId
       * @return {?}
       */
      data.get = function(a, itemId) {
        return 0 < (a = a ? this.S(a) : []).length ? String(a[0]) : itemId;
      };
      /**
       * @return {?}
       */
      data.toString = function() {
        if (this.c) {
          return this.c;
        }
        if (!this.a) {
          return "";
        }
        /** @type {!Array} */
        var buffer = [];
        var displayList = this.a.U();
        /** @type {number} */
        var j = 0;
        for (; j < displayList.length; j++) {
          var el = displayList[j];
          /** @type {string} */
          var urlSafeNodeName = encodeURIComponent(String(el));
          el = this.S(el);
          /** @type {number} */
          var i = 0;
          for (; i < el.length; i++) {
            /** @type {string} */
            var boundary = urlSafeNodeName;
            if ("" !== el[i]) {
              /** @type {string} */
              boundary = boundary + ("=" + encodeURIComponent(String(el[i])));
            }
            buffer.push(boundary);
          }
        }
        return this.c = buffer.join("&");
      };
      /** @type {boolean} */
      var is_defun = !window || 9 <= Number(element);
      /** @type {boolean} */
      ctor.prototype.la = true;
      /**
       * @return {?}
       */
      ctor.prototype.ja = function() {
        return this.a;
      };
      /**
       * @return {?}
       */
      ctor.prototype.toString = function() {
        return "Const{" + this.a + "}";
      };
      var num = {};
      make("");
      /** @type {boolean} */
      Type.prototype.la = true;
      /**
       * @return {?}
       */
      Type.prototype.ja = function() {
        return this.a;
      };
      /**
       * @return {?}
       */
      Type.prototype.toString = function() {
        return "TrustedResourceUrl{" + this.a + "}";
      };
      /** @type {!RegExp} */
      var splitter = /%{(\w+)}/g;
      /** @type {!RegExp} */
      var resource = /^(?:https:)?\/\/[0-9a-z.:[\]-]+\/|^\/[^\/\\]|^about:blank#/i;
      var arg = {};
      /** @type {boolean} */
      type.prototype.la = true;
      /**
       * @return {?}
       */
      type.prototype.ja = function() {
        return this.a;
      };
      /**
       * @return {?}
       */
      type.prototype.toString = function() {
        return "SafeUrl{" + this.a + "}";
      };
      /** @type {!RegExp} */
      var JSON_START = /^(?:(?:https?|mailto|ftp):|[^:/?#]*(?:[/?#]|$))/i;
      var max = {};
      fill("about:blank");
      /** @type {boolean} */
      Obj.prototype.la = true;
      /**
       * @return {?}
       */
      Obj.prototype.ja = function() {
        return this.a;
      };
      /**
       * @return {?}
       */
      Obj.prototype.toString = function() {
        return "SafeHtml{" + this.a + "}";
      };
      var b = {};
      getProp("<!DOCTYPE html>");
      getProp("");
      getProp("<br>");
      var params = {
        cellpadding : "cellPadding",
        cellspacing : "cellSpacing",
        colspan : "colSpan",
        frameborder : "frameBorder",
        height : "height",
        maxlength : "maxLength",
        nonce : "nonce",
        role : "role",
        rowspan : "rowSpan",
        type : "type",
        usemap : "useMap",
        valign : "vAlign",
        width : "width"
      };
      var traced = {
        '"' : '\\"',
        "\\" : "\\\\",
        "/" : "\\/",
        "\b" : "\\b",
        "\f" : "\\f",
        "\n" : "\\n",
        "\r" : "\\r",
        "\t" : "\\t",
        "\x0B" : "\\u000b"
      };
      /** @type {!RegExp} */
      var rx = /\uffff/.test("\uffff") ? /[\\"\x00-\x1f\x7f-\uffff]/g : /[\\"\x00-\x1f\x7f-\xff]/g;
      /** @type {!RegExp} */
      var $jsilxna = /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/;
      /** @type {string} */
      var ios = "Firefox";
      /** @type {string} */
      var months = "Chrome";
      var names = {
        Qc : "FirebaseCore-web",
        Sc : "FirebaseUI-web"
      };
      /**
       * @return {?}
       */
      link.prototype.get = function() {
        var navigator = global.navigator;
        return !navigator || "boolean" != typeof navigator.onLine || !migrateWarn() && "chrome-extension:" !== warn() && void 0 === navigator.connection || navigator.onLine ? this.b ? this.c : this.a : Math.min(5E3, this.a);
      };
      var Ti;
      var Ei = {};
      try {
        var TaskQueueListInstance = {};
        Object.defineProperty(TaskQueueListInstance, "abcd", {
          configurable : true,
          enumerable : true,
          value : 1
        });
        Object.defineProperty(TaskQueueListInstance, "abcd", {
          configurable : true,
          enumerable : true,
          value : 2
        });
        /** @type {boolean} */
        Ti = 2 == TaskQueueListInstance.abcd;
      } catch (Ht) {
        /** @type {boolean} */
        Ti = false;
      }
      /** @type {string} */
      var string = "EMAIL_SIGNIN";
      /** @type {string} */
      var mode = "email";
      /** @type {string} */
      var locale = "newEmail";
      /** @type {string} */
      var codePoint = "requestType";
      /** @type {string} */
      var nameKey = "email";
      /** @type {string} */
      var childName = "fromEmail";
      /** @type {string} */
      var storageName = "data";
      /** @type {string} */
      var provider = "operation";
      expect(Buffer, Error);
      /**
       * @return {?}
       */
      Buffer.prototype.C = function() {
        return {
          code : this.code,
          message : this.message
        };
      };
      /**
       * @return {?}
       */
      Buffer.prototype.toJSON = function() {
        return this.C();
      };
      /** @type {string} */
      var code = "auth/";
      var status = {
        "argument-error" : "",
        "app-not-authorized" : "This app, identified by the domain where it's hosted, is not authorized to use Firebase Authentication with the provided API key. Review your key configuration in the Google API console.",
        "app-not-installed" : "The requested mobile application corresponding to the identifier (Android package name or iOS bundle ID) provided is not installed on this device.",
        "captcha-check-failed" : "The reCAPTCHA response token provided is either invalid, expired, already used or the domain associated with it does not match the list of whitelisted domains.",
        "code-expired" : "The SMS code has expired. Please re-send the verification code to try again.",
        "cordova-not-ready" : "Cordova framework is not ready.",
        "cors-unsupported" : "This browser is not supported.",
        "credential-already-in-use" : "This credential is already associated with a different user account.",
        "custom-token-mismatch" : "The custom token corresponds to a different audience.",
        "requires-recent-login" : "This operation is sensitive and requires recent authentication. Log in again before retrying this request.",
        "dynamic-link-not-activated" : "Please activate Dynamic Links in the Firebase Console and agree to the terms and conditions.",
        "email-already-in-use" : "The email address is already in use by another account.",
        "expired-action-code" : "The action code has expired. ",
        "cancelled-popup-request" : "This operation has been cancelled due to another conflicting popup being opened.",
        "internal-error" : "An internal error has occurred.",
        "invalid-app-credential" : "The phone verification request contains an invalid application verifier. The reCAPTCHA token response is either invalid or expired.",
        "invalid-app-id" : "The mobile app identifier is not registed for the current project.",
        "invalid-user-token" : "This user's credential isn't valid for this project. This can happen if the user's token has been tampered with, or if the user isn't for the project associated with this API key.",
        "invalid-auth-event" : "An internal error has occurred.",
        "invalid-verification-code" : "The SMS verification code used to create the phone auth credential is invalid. Please resend the verification code sms and be sure use the verification code provided by the user.",
        "invalid-continue-uri" : "The continue URL provided in the request is invalid.",
        "invalid-cordova-configuration" : "The following Cordova plugins must be installed to enable OAuth sign-in: cordova-plugin-buildinfo, cordova-universal-links-plugin, cordova-plugin-browsertab, cordova-plugin-inappbrowser and cordova-plugin-customurlscheme.",
        "invalid-custom-token" : "The custom token format is incorrect. Please check the documentation.",
        "invalid-email" : "The email address is badly formatted.",
        "invalid-api-key" : "Your API key is invalid, please check you have copied it correctly.",
        "invalid-cert-hash" : "The SHA-1 certificate hash provided is invalid.",
        "invalid-credential" : "The supplied auth credential is malformed or has expired.",
        "invalid-persistence-type" : "The specified persistence type is invalid. It can only be local, session or none.",
        "invalid-message-payload" : "The email template corresponding to this action contains invalid characters in its message. Please fix by going to the Auth email templates section in the Firebase Console.",
        "invalid-oauth-provider" : "EmailAuthProvider is not supported for this operation. This operation only supports OAuth providers.",
        "invalid-oauth-client-id" : "The OAuth client ID provided is either invalid or does not match the specified API key.",
        "unauthorized-domain" : "This domain is not authorized for OAuth operations for your Firebase project. Edit the list of authorized domains from the Firebase console.",
        "invalid-action-code" : "The action code is invalid. This can happen if the code is malformed, expired, or has already been used.",
        "wrong-password" : "The password is invalid or the user does not have a password.",
        "invalid-phone-number" : "The format of the phone number provided is incorrect. Please enter the phone number in a format that can be parsed into E.164 format. E.164 phone numbers are written in the format [+][country code][subscriber number including area code].",
        "invalid-recipient-email" : "The email corresponding to this action failed to send as the provided recipient email address is invalid.",
        "invalid-sender" : "The email template corresponding to this action contains an invalid sender email or name. Please fix by going to the Auth email templates section in the Firebase Console.",
        "invalid-verification-id" : "The verification ID used to create the phone auth credential is invalid.",
        "missing-android-pkg-name" : "An Android Package Name must be provided if the Android App is required to be installed.",
        "auth-domain-config-required" : "Be sure to include authDomain when calling firebase.initializeApp(), by following the instructions in the Firebase console.",
        "missing-app-credential" : "The phone verification request is missing an application verifier assertion. A reCAPTCHA response token needs to be provided.",
        "missing-verification-code" : "The phone auth credential was created with an empty SMS verification code.",
        "missing-continue-uri" : "A continue URL must be provided in the request.",
        "missing-iframe-start" : "An internal error has occurred.",
        "missing-ios-bundle-id" : "An iOS Bundle ID must be provided if an App Store ID is provided.",
        "missing-phone-number" : "To send verification codes, provide a phone number for the recipient.",
        "missing-verification-id" : "The phone auth credential was created with an empty verification ID.",
        "app-deleted" : "This instance of FirebaseApp has been deleted.",
        "account-exists-with-different-credential" : "An account already exists with the same email address but different sign-in credentials. Sign in using a provider associated with this email address.",
        "network-request-failed" : "A network error (such as timeout, interrupted connection or unreachable host) has occurred.",
        "no-auth-event" : "An internal error has occurred.",
        "no-such-provider" : "User was not linked to an account with the given provider.",
        "operation-not-allowed" : "The given sign-in provider is disabled for this Firebase project. Enable it in the Firebase console, under the sign-in method tab of the Auth section.",
        "operation-not-supported-in-this-environment" : 'This operation is not supported in the environment this application is running on. "location.protocol" must be http, https or chrome-extension and web storage must be enabled.',
        "popup-blocked" : "Unable to establish a connection with the popup. It may have been blocked by the browser.",
        "popup-closed-by-user" : "The popup has been closed by the user before finalizing the operation.",
        "provider-already-linked" : "User can only be linked to one identity for the given provider.",
        "quota-exceeded" : "The project's quota for this operation has been exceeded.",
        "redirect-cancelled-by-user" : "The redirect operation has been cancelled by the user before finalizing.",
        "redirect-operation-pending" : "A redirect sign-in operation is already pending.",
        timeout : "The operation has timed out.",
        "user-token-expired" : "The user's credential is no longer valid. The user must sign in again.",
        "too-many-requests" : "We have blocked all requests from this device due to unusual activity. Try again later.",
        "unauthorized-continue-uri" : "The domain of the continue URL is not whitelisted.  Please whitelist the domain in the Firebase console.",
        "unsupported-persistence-type" : "The current environment does not support the specified persistence type.",
        "user-cancelled" : "User did not grant your application the permissions it requested.",
        "user-not-found" : "There is no user record corresponding to this identifier. The user may have been deleted.",
        "user-disabled" : "The user account has been disabled by an administrator.",
        "user-mismatch" : "The supplied credentials do not correspond to the previously signed in user.",
        "user-signed-out" : "",
        "weak-password" : "The password must be 6 characters long or more.",
        "web-storage-unsupported" : "This browser is not supported or 3rd party cookies and data may be disabled."
      };
      /** @type {string} */
      var k = "android";
      /** @type {string} */
      var aKey = "handleCodeInApp";
      /** @type {string} */
      var step = "iOS";
      /** @type {string} */
      var uri = "url";
      /** @type {string} */
      var token = "installApp";
      /** @type {string} */
      var metricName = "minimumVersion";
      /** @type {string} */
      var what = "packageName";
      /** @type {string} */
      var operator = "bundleId";
      /** @type {null} */
      var hash = null;
      /** @type {null} */
      var o = null;
      /**
       * @return {?}
       */
      Foo.prototype.f = function() {
        return this.b;
      };
      /** @type {!Array<string>} */
      var onResponse = "oauth_consumer_key oauth_nonce oauth_signature oauth_signature_method oauth_timestamp oauth_token oauth_version".split(" ");
      /** @type {!Array} */
      var props = ["client_id", "response_type", "scope", "redirect_uri", "state"];
      var bookIDs = {
        Rc : {
          Ma : "locale",
          Aa : 500,
          za : 600,
          Na : "facebook.com",
          cb : props
        },
        Tc : {
          Ma : null,
          Aa : 500,
          za : 620,
          Na : "github.com",
          cb : props
        },
        Uc : {
          Ma : "hl",
          Aa : 515,
          za : 680,
          Na : "google.com",
          cb : props
        },
        $c : {
          Ma : "lang",
          Aa : 485,
          za : 705,
          Na : "twitter.com",
          cb : onResponse
        }
      };
      /** @type {string} */
      var j = "idToken";
      /** @type {string} */
      var property = "providerId";
      expect(settings, describe);
      expect(parseError, settings);
      expect(Provider, settings);
      expect(successCb, settings);
      expect(tag, settings);
      /**
       * @param {!Object} key
       * @return {?}
       */
      options.prototype.xa = function(key) {
        return getApi(key, doRequest(this));
      };
      /**
       * @param {!Object} id
       * @param {string} token
       * @return {?}
       */
      options.prototype.c = function(id, token) {
        var response = doRequest(this);
        return response.idToken = token, children(id, response);
      };
      /**
       * @param {!Object} a
       * @param {string} b
       * @return {?}
       */
      options.prototype.f = function(a, b) {
        return t(Collection(a, doRequest(this)), b);
      };
      /**
       * @return {?}
       */
      options.prototype.C = function() {
        var params = {
          providerId : this.providerId,
          signInMethod : this.signInMethod
        };
        return this.idToken && (params.oauthIdToken = this.idToken), this.accessToken && (params.oauthAccessToken = this.accessToken), this.secret && (params.oauthTokenSecret = this.secret), params;
      };
      /**
       * @param {!Array} p
       * @return {?}
       */
      g.prototype.Ca = function(p) {
        return this.tb = clamp(p), this;
      };
      expect(self, g);
      /**
       * @param {string} url
       * @return {?}
       */
      self.prototype.ta = function(url) {
        return push(this.a, url) || this.a.push(url), this;
      };
      /**
       * @return {?}
       */
      self.prototype.yb = function() {
        return require(this.a);
      };
      /**
       * @param {string} email
       * @param {string} password
       * @return {?}
       */
      self.prototype.credential = function(email, password) {
        if (!email && !password) {
          throw new Buffer("argument-error", "credential failed: must provide the ID token and/or the access token.");
        }
        return new options(this.providerId, {
          idToken : email || null,
          accessToken : password || null
        }, this.providerId);
      };
      expect(cache, self);
      debug(cache, "PROVIDER_ID", "facebook.com");
      debug(cache, "FACEBOOK_SIGN_IN_METHOD", "facebook.com");
      expect(array, self);
      debug(array, "PROVIDER_ID", "github.com");
      debug(array, "GITHUB_SIGN_IN_METHOD", "github.com");
      expect(def, self);
      debug(def, "PROVIDER_ID", "google.com");
      debug(def, "GOOGLE_SIGN_IN_METHOD", "google.com");
      expect(template, g);
      debug(template, "PROVIDER_ID", "twitter.com");
      debug(template, "TWITTER_SIGN_IN_METHOD", "twitter.com");
      /**
       * @param {!Object} t
       * @return {?}
       */
      Template.prototype.xa = function(t) {
        return this.signInMethod == item.EMAIL_LINK_SIGN_IN_METHOD ? callback(t, obj, {
          email : this.a,
          oobCode : this.b
        }) : callback(t, found, {
          email : this.a,
          password : this.b
        });
      };
      /**
       * @param {!Object} val
       * @param {string} id
       * @return {?}
       */
      Template.prototype.c = function(val, id) {
        return this.signInMethod == item.EMAIL_LINK_SIGN_IN_METHOD ? callback(val, opt, {
          idToken : id,
          email : this.a,
          oobCode : this.b
        }) : callback(val, colors, {
          idToken : id,
          email : this.a,
          password : this.b
        });
      };
      /**
       * @param {!Object} a
       * @param {string} b
       * @return {?}
       */
      Template.prototype.f = function(a, b) {
        return t(this.xa(a), b);
      };
      /**
       * @return {?}
       */
      Template.prototype.C = function() {
        return {
          email : this.a,
          password : this.b,
          signInMethod : this.signInMethod
        };
      };
      done(item, {
        PROVIDER_ID : "password"
      });
      done(item, {
        EMAIL_LINK_SIGN_IN_METHOD : "emailLink"
      });
      done(item, {
        EMAIL_PASSWORD_SIGN_IN_METHOD : "password"
      });
      /**
       * @param {!Object} strip1
       * @return {?}
       */
      source.prototype.xa = function(strip1) {
        return strip1.Sa(Transform(this));
      };
      /**
       * @param {!Object} t
       * @param {string} e
       * @return {?}
       */
      source.prototype.c = function(t, e) {
        var b = Transform(this);
        return b.idToken = e, callback(t, spec, b);
      };
      /**
       * @param {!Object} value
       * @param {string} width
       * @return {?}
       */
      source.prototype.f = function(value, width) {
        var c = Transform(this);
        return c.operation = "REAUTH", t(value = callback(value, ret, c), width);
      };
      /**
       * @return {?}
       */
      source.prototype.C = function() {
        var options = {
          providerId : "phone"
        };
        return this.a.Ra && (options.verificationId = this.a.Ra), this.a.Qa && (options.verificationCode = this.a.Qa), this.a.Ea && (options.temporaryProof = this.a.Ea), this.a.Z && (options.phoneNumber = this.a.Z), options;
      };
      /**
       * @param {string} phone
       * @param {!Object} messages
       * @return {?}
       */
      d.prototype.Sa = function(phone, messages) {
        var f = this.a.b;
        return resolve(messages.verify()).then(function(i) {
          if (!isString(i)) {
            throw new Buffer("argument-error", "An implementation of firebase.auth.ApplicationVerifier.prototype.verify() must return a firebase.Promise that resolves with a string.");
          }
          switch(messages.type) {
            case "recaptcha":
              return function(i, e) {
                return callback(i, pairs, e);
              }(f, {
                phoneNumber : phone,
                recaptchaToken : i
              }).then(function(canCreateDiscussions) {
                return "function" == typeof messages.reset && messages.reset(), canCreateDiscussions;
              }, function(canCreateDiscussions) {
                throw "function" == typeof messages.reset && messages.reset(), canCreateDiscussions;
              });
            default:
              throw new Buffer("argument-error", 'Only firebase.auth.ApplicationVerifiers with type="recaptcha" are currently supported.');
          }
        });
      };
      done(d, {
        PROVIDER_ID : "phone"
      });
      done(d, {
        PHONE_SIGN_IN_METHOD : "phone"
      });
      /**
       * @return {?}
       */
      Node.prototype.C = function() {
        return {
          type : this.b,
          eventId : this.c,
          urlResponse : this.f,
          sessionId : this.g,
          error : this.a && this.a.C()
        };
      };
      /** @type {null} */
      var placeholder = null;
      /**
       * @param {string} obj
       * @return {undefined}
       */
      tester.prototype.subscribe = function(obj) {
        var e = this;
        this.a.push(obj);
        if (!this.b) {
          /**
           * @param {!Object} val
           * @return {undefined}
           */
          this.b = function(val) {
            /** @type {number} */
            var i = 0;
            for (; i < e.a.length; i++) {
              e.a[i](val);
            }
          };
          if ("function" == typeof(obj = stringify("universalLinks.subscribe", global))) {
            obj(null, this.b);
          }
        }
      };
      /**
       * @param {?} stream
       * @return {undefined}
       */
      tester.prototype.unsubscribe = function(stream) {
        s(this.a, function(url) {
          return url == stream;
        });
      };
      expect(ui, Buffer);
      expect(Comment, Buffer);
      /**
       * @return {?}
       */
      Comment.prototype.C = function() {
        var data = {
          code : this.code,
          message : this.message
        };
        if (this.email) {
          data.email = this.email;
        }
        if (this.phoneNumber) {
          data.phoneNumber = this.phoneNumber;
        }
        var token = this.credential && this.credential.C();
        return token && log(data, token), data;
      };
      /**
       * @return {?}
       */
      Comment.prototype.toJSON = function() {
        return this.C();
      };
      var value;
      /** @type {!RegExp} */
      var validator = /^[+a-zA-Z0-9_.!#$%&'*\/=?^`{|}~-]+@([a-zA-Z0-9-]+\.)+[a-zA-Z0-9]{2,63}$/;
      /** @type {null} */
      src.prototype.c = null;
      expect(testObject, src);
      /**
       * @return {?}
       */
      testObject.prototype.a = function() {
        var str = inspect(this);
        return str ? new ActiveXObject(str) : new XMLHttpRequest;
      };
      /**
       * @return {?}
       */
      testObject.prototype.b = function() {
        var t = {};
        return inspect(this) && (t[0] = true, t[1] = true), t;
      };
      value = new testObject;
      expect(cc, src);
      /**
       * @return {?}
       */
      cc.prototype.a = function() {
        /** @type {!XMLHttpRequest} */
        var xhr = new XMLHttpRequest;
        if ("withCredentials" in xhr) {
          return xhr;
        }
        if ("undefined" != typeof XDomainRequest) {
          return new xhr;
        }
        throw Error("Unsupported browser");
      };
      /**
       * @return {?}
       */
      cc.prototype.b = function() {
        return {};
      };
      /**
       * @param {!Object} a
       * @param {string} b
       * @param {string} n
       * @return {undefined}
       */
      (data = xhr.prototype).open = function(a, b, n) {
        if (null != n && !n) {
          throw Error("Only async requests are supported.");
        }
        this.a.open(a, b);
      };
      /**
       * @param {!Object} a
       * @return {undefined}
       */
      data.send = function(a) {
        if (a) {
          if ("string" != typeof a) {
            throw Error("Only string data is supported");
          }
          this.a.send(a);
        } else {
          this.a.send();
        }
      };
      /**
       * @return {undefined}
       */
      data.abort = function() {
        this.a.abort();
      };
      /**
       * @return {undefined}
       */
      data.setRequestHeader = function() {
      };
      /**
       * @param {string} name
       * @return {?}
       */
      data.getResponseHeader = function(name) {
        return "content-type" == name.toLowerCase() ? this.a.contentType : "";
      };
      /**
       * @return {undefined}
       */
      data.bc = function() {
        /** @type {number} */
        this.status = 200;
        this.responseText = this.a.responseText;
        readyStateChange(this, 4);
      };
      /**
       * @return {undefined}
       */
      data.zb = function() {
        /** @type {number} */
        this.status = 500;
        /** @type {string} */
        this.responseText = "";
        readyStateChange(this, 4);
      };
      /**
       * @return {undefined}
       */
      data.fc = function() {
        this.zb();
      };
      /**
       * @return {undefined}
       */
      data.cc = function() {
        /** @type {number} */
        this.status = 200;
        readyStateChange(this, 1);
      };
      /**
       * @return {?}
       */
      data.getAllResponseHeaders = function() {
        return "content-type: " + this.a.contentType;
      };
      /** @type {null} */
      Parser.prototype.a = null;
      /**
       * @param {?} propertyName
       * @param {?} percent
       * @param {?} callback
       * @param {!Object} position
       * @param {!Object} rotation
       * @return {undefined}
       */
      Parser.prototype.reset = function(propertyName, percent, callback, position, rotation) {
        delete this.a;
      };
      /**
       * @return {?}
       */
      Param.prototype.toString = function() {
        return this.name;
      };
      var str = new Param("SEVERE", 1E3);
      var href = new Param("WARNING", 900);
      var parent = new Param("CONFIG", 700);
      var param = new Param("FINE", 500);
      /**
       * @param {string} p
       * @param {!Object} f
       * @param {!Object} a
       * @return {undefined}
       */
      Constraint.prototype.log = function(p, f, a) {
        if (p.value >= function clean(d) {
          return d.c ? d.c : d.a ? clean(d.a) : (sort("Root logger has no level set."), null);
        }(this).value) {
          if (isNaN(f)) {
            f = f();
          }
          p = new Parser(p, String(f), this.f);
          if (a) {
            /** @type {!Object} */
            p.a = a;
          }
          a = this;
          for (; a;) {
            a = a.a;
          }
        }
      };
      var buffer = {};
      /** @type {null} */
      var c = null;
      expect(Worker, src);
      /**
       * @return {?}
       */
      Worker.prototype.a = function() {
        return new f(this.f);
      };
      Worker.prototype.b = function(partKeys) {
        return function() {
          return partKeys;
        };
      }({});
      expect(f, p);
      /** @type {number} */
      var CLOSED = 0;
      /**
       * @param {!Function} key
       * @param {string} value
       * @return {undefined}
       */
      (data = f.prototype).open = function(key, value) {
        if (this.readyState != CLOSED) {
          throw this.abort(), Error("Error reopening a connection");
        }
        /** @type {!Function} */
        this.h = key;
        /** @type {string} */
        this.c = value;
        /** @type {number} */
        this.readyState = 1;
        setState(this);
      };
      /**
       * @param {!Object} fields
       * @return {undefined}
       */
      data.send = function(fields) {
        if (1 != this.readyState) {
          throw this.abort(), Error("need to call open() first. ");
        }
        /** @type {boolean} */
        this.a = true;
        var options = {
          headers : this.g,
          method : this.h,
          credentials : void 0,
          cache : void 0
        };
        if (fields) {
          /** @type {!Object} */
          options.body = fields;
        }
        this.j.fetch(new Request(this.c, options)).then(this.ec.bind(this), this.Ab.bind(this));
      };
      /**
       * @return {undefined}
       */
      data.abort = function() {
        /** @type {string} */
        this.responseText = "";
        /** @type {!Headers} */
        this.g = new Headers;
        /** @type {number} */
        this.status = 0;
        if (1 <= this.readyState && this.a && 4 != this.readyState) {
          /** @type {number} */
          this.readyState = 4;
          /** @type {boolean} */
          this.a = false;
          setState(this);
        }
        /** @type {number} */
        this.readyState = CLOSED;
      };
      /**
       * @param {!Object} params
       * @return {undefined}
       */
      data.ec = function(params) {
        if (this.a) {
          if (!this.b) {
            this.b = params.headers;
            /** @type {number} */
            this.readyState = 2;
            setState(this);
          }
          if (this.a) {
            /** @type {number} */
            this.readyState = 3;
            setState(this);
            if (this.a) {
              params.text().then(this.dc.bind(this, params), this.Ab.bind(this));
            }
          }
        }
      };
      /**
       * @param {!Object} res
       * @param {string} n
       * @return {undefined}
       */
      data.dc = function(res, n) {
        if (this.a) {
          this.status = res.status;
          this.statusText = res.statusText;
          /** @type {string} */
          this.responseText = n;
          /** @type {number} */
          this.readyState = 4;
          setState(this);
        }
      };
      /**
       * @param {?} reason
       * @return {undefined}
       */
      data.Ab = function(reason) {
        var async = this.f;
        if (async) {
          async.log(href, "Failed to fetch url " + this.c, reason instanceof Error ? reason : Error(reason));
        }
        if (this.a) {
          /** @type {number} */
          this.readyState = 4;
          setState(this);
        }
      };
      /**
       * @param {?} value
       * @param {?} key
       * @return {undefined}
       */
      data.setRequestHeader = function(value, key) {
        this.g.append(value, key);
      };
      /**
       * @param {!Object} match
       * @return {?}
       */
      data.getResponseHeader = function(match) {
        return this.b ? this.b.get(match.toLowerCase()) || "" : ((match = this.f) && match.log(href, "Attempting to get response header but no headers have been received for url: " + this.c, void 0), "");
      };
      /**
       * @return {?}
       */
      data.getAllResponseHeaders = function() {
        if (!this.b) {
          var fields = this.f;
          return fields && fields.log(href, "Attempting to get all response headers but no headers have been received for url: " + this.c, void 0), "";
        }
        /** @type {!Array} */
        fields = [];
        var deletedChar = this.b.entries();
        var tmp = deletedChar.next();
        for (; !tmp.done;) {
          tmp = tmp.value;
          fields.push(tmp[0] + ": " + tmp[1]);
          tmp = deletedChar.next();
        }
        return fields.join("\r\n");
      };
      expect(node, p);
      /** @type {string} */
      var other = "";
      node.prototype.b = parser("goog.net.XhrIo");
      /** @type {!RegExp} */
      var alpha = /^https?$/i;
      /** @type {!Array} */
      var methods = ["POST", "PUT"];
      /**
       * @return {undefined}
       */
      (data = node.prototype).Fa = function() {
        if (void 0 !== Directory && this.a) {
          /** @type {string} */
          this.h = "Timed out after " + this.g + "ms, aborting";
          runner(this.b, right(this, this.h));
          this.dispatchEvent("timeout");
          this.abort(8);
        }
      };
      /**
       * @return {undefined}
       */
      data.abort = function() {
        if (this.a && this.c) {
          runner(this.b, right(this, "Aborting"));
          /** @type {boolean} */
          this.c = false;
          /** @type {boolean} */
          this.f = true;
          this.a.abort();
          /** @type {boolean} */
          this.f = false;
          this.dispatchEvent("complete");
          this.dispatchEvent("abort");
          abort(this);
        }
      };
      /**
       * @return {undefined}
       */
      data.ua = function() {
        if (this.a) {
          if (this.c) {
            /** @type {boolean} */
            this.c = false;
            /** @type {boolean} */
            this.f = true;
            this.a.abort();
            /** @type {boolean} */
            this.f = false;
          }
          abort(this, true);
        }
        node.lb.ua.call(this);
      };
      /**
       * @return {undefined}
       */
      data.Db = function() {
        if (!this.pa) {
          if (this.I || this.j || this.f) {
            next(this);
          } else {
            this.tc();
          }
        }
      };
      /**
       * @return {undefined}
       */
      data.tc = function() {
        next(this);
      };
      /**
       * @return {?}
       */
      data.getResponse = function() {
        try {
          if (!this.a) {
            return null;
          }
          if ("response" in this.a) {
            return this.a.response;
          }
          switch(this.o) {
            case other:
            case "text":
              return this.a.responseText;
            case "arraybuffer":
              if ("mozResponseArrayBuffer" in this.a) {
                return this.a.mozResponseArrayBuffer;
              }
          }
          var b = this.b;
          return b && b.log(str, "Response type " + this.o + " is not supported on this browser", void 0), null;
        } catch (controlFlowAction) {
          return runner(this.b, "Can not get response: " + controlFlowAction.message), null;
        }
      };
      /**
       * @param {string} data
       * @return {undefined}
       */
      m.prototype.cancel = function(data) {
        if (this.a) {
          if (this.c instanceof m) {
            this.c.cancel();
          }
        } else {
          if (this.b) {
            var e = this.b;
            delete this.b;
            if (data) {
              e.cancel(data);
            } else {
              e.l--;
              if (0 >= e.l) {
                e.cancel();
              }
            }
          }
          if (this.v) {
            this.v.call(this.s, this);
          } else {
            /** @type {boolean} */
            this.u = true;
          }
          if (!this.a) {
            data = new Tag(this);
            show(this);
            w(this, false, data);
          }
        }
      };
      /**
       * @param {!Object} t
       * @param {string} o
       * @return {undefined}
       */
      m.prototype.o = function(t, o) {
        /** @type {boolean} */
        this.j = false;
        w(this, t, o);
      };
      /**
       * @return {undefined}
       */
      m.prototype.D = function() {
        show(this);
        w(this, true, null);
      };
      /**
       * @param {!Function} value
       * @param {!Function} fn
       * @param {!Object} callback
       * @return {?}
       */
      m.prototype.then = function(value, fn, callback) {
        var options;
        var isPromise;
        var b = new Promise(function(calendarNameOrOptionsObject, isSlidingUp) {
          options = calendarNameOrOptionsObject;
          isPromise = isSlidingUp;
        });
        return after(this, options, function(data) {
          if (data instanceof Tag) {
            b.cancel();
          } else {
            isPromise(data);
          }
        }), b.then(value, fn, callback);
      };
      defineProperty(m);
      expect(Notification, Constructor);
      /** @type {string} */
      Notification.prototype.message = "Deferred has already fired";
      /** @type {string} */
      Notification.prototype.name = "AlreadyCalledError";
      expect(Tag, Constructor);
      /** @type {string} */
      Tag.prototype.message = "Deferred was canceled";
      /** @type {string} */
      Tag.prototype.name = "CanceledError";
      /**
       * @return {?}
       */
      Person.prototype.c = function() {
        throw delete elements[this.a], this.b;
      };
      var elements = {};
      /** @type {number} */
      var rawResponse = 0;
      /** @type {number} */
      var defaultPath = 1;
      expect(Response, Constructor);
      expect(a, src);
      /**
       * @return {?}
       */
      a.prototype.a = function() {
        return new this.f;
      };
      /**
       * @return {?}
       */
      a.prototype.b = function() {
        return {};
      };
      var serverRoutes;
      /** @type {string} */
      var i = "idToken";
      var EMPTY = new link(3E4, 6E4);
      var header = {
        "Content-Type" : "application/x-www-form-urlencoded"
      };
      var defaultPriority = new link(3E4, 6E4);
      var JSON_CONTENT_TYPE_HEADER = {
        "Content-Type" : "application/json"
      };
      /**
       * @param {!Object} val
       * @param {string} r
       * @param {string} type
       * @param {string} id
       * @param {string} name
       * @param {?} arg
       * @return {undefined}
       */
      Player.prototype.o = function(val, r, type, id, name, arg) {
        if (replace() && (void 0 === global.fetch || void 0 === global.Headers || void 0 === global.Request)) {
          throw new Buffer("operation-not-supported-in-this-environment", "fetch, Headers and Request native APIs or equivalent Polyfills must be available to support HTTP requests from a Worker environment.");
        }
        var c = new node(this.c);
        if (arg) {
          /** @type {number} */
          c.g = Math.max(0, arg);
          /** @type {number} */
          var d = setTimeout(function() {
            c.dispatchEvent("timeout");
          }, arg);
        }
        register(c, "complete", function() {
          if (d) {
            clearTimeout(d);
          }
          /** @type {null} */
          var G_MOD = null;
          try {
            /** @type {*} */
            G_MOD = JSON.parse(function(resp) {
              try {
                return resp.a ? resp.a.responseText : "";
              } catch (controlFlowAction) {
                return runner(resp.b, "Can not get responseText: " + controlFlowAction.message), "";
              }
            }(this)) || null;
          } catch (e) {
            /** @type {null} */
            G_MOD = null;
          }
          if (r) {
            r(G_MOD);
          }
        });
        append(c, "ready", function() {
          if (d) {
            clearTimeout(d);
          }
          exec(this);
        });
        append(c, "timeout", function() {
          if (d) {
            clearTimeout(d);
          }
          exec(this);
          if (r) {
            r(null);
          }
        });
        get(c, val, type, id, name);
      };
      var row = make("https://apis.google.com/js/client.js?onload=%{onload}");
      /** @type {string} */
      var evsImportName = "__fcb" + Math.floor(1E6 * Math.random()).toString();
      /**
       * @param {string} url
       * @param {?} callback
       * @param {!Object} name
       * @param {!Object} content
       * @param {!Object} props
       * @return {undefined}
       */
      Player.prototype.u = function(url, callback, name, content, props) {
        var keys = this;
        serverRoutes.then(function() {
          window.gapi.client.setApiKey(keys.b);
          var value = window.gapi.auth.getToken();
          window.gapi.auth.setToken(null);
          window.gapi.client.request({
            path : url,
            method : name,
            body : content,
            headers : props,
            authType : "none",
            callback : function(sessionAttributes) {
              window.gapi.auth.setToken(value);
              if (callback) {
                callback(sessionAttributes);
              }
            }
          });
        }).m(function(storeErr) {
          if (callback) {
            callback({
              error : {
                message : storeErr && storeErr.message || "CORS_UNSUPPORTED"
              }
            });
          }
        });
      };
      /**
       * @return {?}
       */
      Player.prototype.jb = function() {
        return callback(this, option, {});
      };
      /**
       * @param {string} size
       * @param {string} key
       * @return {?}
       */
      Player.prototype.mb = function(size, key) {
        return callback(this, keys, {
          idToken : size,
          email : key
        });
      };
      /**
       * @param {!Object} done
       * @param {!Object} o
       * @return {?}
       */
      Player.prototype.nb = function(done, o) {
        return callback(this, colors, {
          idToken : done,
          password : o
        });
      };
      var number = {
        displayName : "DISPLAY_NAME",
        photoUrl : "PHOTO_URL"
      };
      /**
       * @param {!Object} id
       * @param {!Object} target
       * @return {?}
       */
      (data = Player.prototype).ob = function(id, target) {
        var result = {
          idToken : id
        };
        /** @type {!Array} */
        var i = [];
        return attr(number, function(boost, k) {
          var v = target[k];
          if (null === v) {
            i.push(boost);
          } else {
            if (k in target) {
              result[k] = v;
            }
          }
        }), i.length && (result.deleteAttribute = i), callback(this, keys, result);
      };
      /**
       * @param {!Object} options
       * @param {?} opts
       * @return {?}
       */
      data.gb = function(options, opts) {
        return log(options = {
          requestType : "PASSWORD_RESET",
          email : options
        }, opts), callback(this, _data, options);
      };
      /**
       * @param {!Object} options
       * @param {?} id
       * @return {?}
       */
      data.hb = function(options, id) {
        return log(options = {
          requestType : "EMAIL_SIGNIN",
          email : options
        }, id), callback(this, context, options);
      };
      /**
       * @param {!Object} e
       * @param {?} options
       * @return {?}
       */
      data.fb = function(e, options) {
        return log(e = {
          requestType : "VERIFY_EMAIL",
          idToken : e
        }, options), callback(this, out, e);
      };
      /**
       * @param {!Object} t
       * @return {?}
       */
      data.Sa = function(t) {
        return callback(this, opts, t);
      };
      /**
       * @param {?} type
       * @param {string} data
       * @return {?}
       */
      data.Wa = function(type, data) {
        return callback(this, original, {
          oobCode : type,
          newPassword : data
        });
      };
      /**
       * @param {!Function} enterTime
       * @return {?}
       */
      data.Ia = function(enterTime) {
        return callback(this, values, {
          oobCode : enterTime
        });
      };
      /**
       * @param {?} enterTime
       * @return {?}
       */
      data.Ua = function(enterTime) {
        return callback(this, defaults, {
          oobCode : enterTime
        });
      };
      var defaults = {
        endpoint : "setAccountInfo",
        A : testExtBufferArray,
        ba : "email"
      };
      var values = {
        endpoint : "resetPassword",
        A : testExtBufferArray,
        J : function(c) {
          var type = c.requestType;
          if (!type || !c.email && "EMAIL_SIGNIN" != type) {
            throw new Buffer("internal-error");
          }
        }
      };
      var that = {
        endpoint : "signupNewUser",
        A : function(t) {
          if (T(t), !t.password) {
            throw new Buffer("weak-password");
          }
        },
        J : getDate,
        R : true
      };
      var properties = {
        endpoint : "createAuthUri"
      };
      var table = {
        endpoint : "deleteAccount",
        T : ["idToken"]
      };
      var service = {
        endpoint : "setAccountInfo",
        T : ["idToken", "deleteProvider"],
        A : function(t) {
          if (!each(t.deleteProvider)) {
            throw new Buffer("internal-error");
          }
        }
      };
      var obj = {
        endpoint : "emailLinkSignin",
        T : ["email", "oobCode"],
        A : T,
        J : getDate,
        R : true
      };
      var opt = {
        endpoint : "emailLinkSignin",
        T : ["idToken", "email", "oobCode"],
        A : T,
        J : getDate,
        R : true
      };
      var response = {
        endpoint : "getAccountInfo"
      };
      var context = {
        endpoint : "getOobConfirmationCode",
        T : ["requestType"],
        A : function(t) {
          if ("EMAIL_SIGNIN" != t.requestType) {
            throw new Buffer("internal-error");
          }
          T(t);
        },
        ba : "email"
      };
      var out = {
        endpoint : "getOobConfirmationCode",
        T : ["idToken", "requestType"],
        A : function(obj) {
          if ("VERIFY_EMAIL" != obj.requestType) {
            throw new Buffer("internal-error");
          }
        },
        ba : "email"
      };
      var _data = {
        endpoint : "getOobConfirmationCode",
        T : ["requestType"],
        A : function(t) {
          if ("PASSWORD_RESET" != t.requestType) {
            throw new Buffer("internal-error");
          }
          T(t);
        },
        ba : "email"
      };
      var packet = {
        pb : true,
        endpoint : "getProjectConfig",
        Cb : "GET"
      };
      var user = {
        pb : true,
        endpoint : "getRecaptchaParam",
        Cb : "GET",
        J : function(b) {
          if (!b.recaptchaSiteKey) {
            throw new Buffer("internal-error");
          }
        }
      };
      var original = {
        endpoint : "resetPassword",
        A : testExtBufferArray,
        ba : "email"
      };
      var pairs = {
        endpoint : "sendVerificationCode",
        T : ["phoneNumber", "recaptchaToken"],
        ba : "sessionInfo"
      };
      var keys = {
        endpoint : "setAccountInfo",
        T : ["idToken"],
        A : y,
        R : true
      };
      var colors = {
        endpoint : "setAccountInfo",
        T : ["idToken"],
        A : function(a) {
          if (y(a), !a.password) {
            throw new Buffer("weak-password");
          }
        },
        J : getDate,
        R : true
      };
      var option = {
        endpoint : "signupNewUser",
        J : getDate,
        R : true
      };
      var subscription = {
        endpoint : "verifyAssertion",
        A : Request,
        J : J,
        R : true
      };
      var types = {
        endpoint : "verifyAssertion",
        A : Request,
        J : function(d) {
          if (d.errorMessage && "USER_NOT_FOUND" == d.errorMessage) {
            throw new Buffer("user-not-found");
          }
          if (d.errorMessage) {
            throw $$(d.errorMessage);
          }
          if (!d[i]) {
            throw new Buffer("internal-error");
          }
        },
        R : true
      };
      var formats = {
        endpoint : "verifyAssertion",
        A : function(result) {
          if (Request(result), !result.idToken) {
            throw new Buffer("internal-error");
          }
        },
        J : J,
        R : true
      };
      var state = {
        endpoint : "verifyCustomToken",
        A : function(type) {
          if (!type.token) {
            throw new Buffer("invalid-custom-token");
          }
        },
        J : getDate,
        R : true
      };
      var found = {
        endpoint : "verifyPassword",
        A : function(t) {
          if (T(t), !t.password) {
            throw new Buffer("wrong-password");
          }
        },
        J : getDate,
        R : true
      };
      var opts = {
        endpoint : "verifyPhoneNumber",
        A : handleError,
        J : getDate
      };
      var spec = {
        endpoint : "verifyPhoneNumber",
        A : function(result) {
          if (!result.idToken) {
            throw new Buffer("internal-error");
          }
          handleError(result);
        },
        J : function(d) {
          if (d.temporaryProof) {
            throw d.code = "credential-already-in-use", serialize(d);
          }
          getDate(d);
        }
      };
      var ret = {
        Tb : {
          USER_NOT_FOUND : "user-not-found"
        },
        endpoint : "verifyPhoneNumber",
        A : handleError,
        J : getDate
      };
      var project;
      var reqs = {
        Wc : {
          Ya : "https://www.googleapis.com/identitytoolkit/v3/relyingparty/",
          eb : "https://securetoken.googleapis.com/v1/token",
          id : "p"
        },
        Yc : {
          Ya : "https://staging-www.sandbox.googleapis.com/identitytoolkit/v3/relyingparty/",
          eb : "https://staging-securetoken.sandbox.googleapis.com/v1/token",
          id : "s"
        },
        Zc : {
          Ya : "https://www-googleapis-test.sandbox.google.com/identitytoolkit/v3/relyingparty/",
          eb : "https://test-securetoken.sandbox.googleapis.com/v1/token",
          id : "t"
        }
      };
      /** @type {(string|undefined)} */
      project = join("__EID__") ? "__EID__" : void 0;
      var _button;
      var left = make("https://apis.google.com/js/api.js?onload=%{onload}");
      var clickedGroup = new link(3E4, 6E4);
      var readerAPI = new link(5E3, 15E3);
      /** @type {null} */
      var offset = null;
      /**
       * @return {?}
       */
      Color.prototype.toString = function() {
        return this.f ? func(this.a, "v", this.f) : _(this.a.a, "v"), this.b ? func(this.a, "eid", this.b) : _(this.a.a, "eid"), this.c.length ? func(this.a, "fw", this.c.join(",")) : _(this.a.a, "fw"), this.a.toString();
      };
      /**
       * @return {?}
       */
      constructor.prototype.toString = function() {
        var d = setColor(this.o, "/__/auth/handler");
        if (func(d, "apiKey", this.u), func(d, "appName", this.c), func(d, "authType", this.l), this.a.isOAuthProvider) {
          var c = this.a;
          try {
            var b = config.app(this.c).auth().ca();
          } catch (t) {
            /** @type {null} */
            b = null;
          }
          var r;
          for (r in c.Xa = b, func(d, "providerId", this.a.providerId), b = $getDRtoDL((c = this.a).tb)) {
            b[r] = b[r].toString();
          }
          r = c.Ac;
          b = clamp(b);
          /** @type {number} */
          var i = 0;
          for (; i < r.length; i++) {
            var name = r[i];
            if (name in b) {
              delete b[name];
            }
          }
          if (c.Za && c.Xa && !b[c.Za]) {
            b[c.Za] = c.Xa;
          }
          if (!isMatch(b)) {
            func(d, "customParameters", parse(b));
          }
        }
        if ("function" == typeof this.a.yb && ((c = this.a.yb()).length && func(d, "scopes", c.join(","))), this.j ? func(d, "redirectUrl", this.j) : _(d.a, "redirectUrl"), this.g ? func(d, "eventId", this.g) : _(d.a, "eventId"), this.h ? func(d, "v", this.h) : _(d.a, "v"), this.b) {
          var i;
          for (i in this.b) {
            if (this.b.hasOwnProperty(i) && !isFinite(d, i)) {
              func(d, i, this.b[i]);
            }
          }
        }
        return this.f ? func(d, "eid", this.f) : _(d.a, "eid"), (i = concat(this.c)).length && func(d, "fw", i.join(",")), d.toString();
      };
      /**
       * @param {!Object} elem
       * @param {string} e
       * @param {undefined} data
       * @return {?}
       */
      (data = calc.prototype).Da = function(elem, e, data) {
        var B = new Buffer("popup-closed-by-user");
        var a = new Buffer("web-storage-unsupported");
        var o = this;
        /** @type {boolean} */
        var r = false;
        return this.ea().then(function() {
          (function(data) {
            var e = {
              type : "webStorageSupport"
            };
            return message(data).then(function() {
              return function(state, obj) {
                return state.ab.then(function() {
                  return new Promise(function(customfont) {
                    state.a.send(obj.type, obj, customfont, stringify("gapi.iframes.CROSS_ORIGIN_IFRAMES_FILTER"));
                  });
                });
              }(data.j, e);
            }).then(function(connectionConfigs) {
              if (connectionConfigs && connectionConfigs.length && void 0 !== connectionConfigs[0].webStorageSupport) {
                return connectionConfigs[0].webStorageSupport;
              }
              throw Error();
            });
          })(o).then(function(n) {
            if (!n) {
              if (elem) {
                isObject(elem);
              }
              e(a);
              /** @type {boolean} */
              r = true;
            }
          });
        }).m(function() {
        }).then(function() {
          if (!r) {
            return function(elem) {
              return new Promise(function(exec) {
                return function authenticate() {
                  validate(2E3).then(function() {
                    if (elem && !elem.closed) {
                      return authenticate();
                    }
                    exec();
                  });
                }();
              });
            }(elem);
          }
        }).then(function() {
          if (!r) {
            return validate(data).then(function() {
              e(B);
            });
          }
        });
      };
      /**
       * @return {?}
       */
      data.Gb = function() {
        var name = end();
        return !query(name) && !process(name);
      };
      /**
       * @return {?}
       */
      data.Bb = function() {
        return false;
      };
      /**
       * @param {string} col
       * @param {string} value
       * @param {!Object} type
       * @param {!Function} fn
       * @param {!Function} cb
       * @param {string} data
       * @param {?} marker
       * @return {?}
       */
      data.xb = function(col, value, type, fn, cb, data, marker) {
        if (!col) {
          return cb(new Buffer("popup-blocked"));
        }
        if (marker && !query()) {
          return this.ea().m(function(fallbackReleases) {
            isObject(col);
            cb(fallbackReleases);
          }), fn(), resolve();
        }
        if (!this.a) {
          this.a = postLink(reset(this));
        }
        var result = this;
        return this.a.then(function() {
          var res = result.ea().m(function(errReadDir) {
            throw isObject(col), cb(errReadDir), errReadDir;
          });
          return fn(), res;
        }).then(function() {
          if (!(rewrite(type), marker)) {
            finish(format(result.u, result.f, result.b, value, type, null, data, result.c, void 0, result.h), col);
          }
        }).m(function(apiResponseError) {
          throw "auth/network-request-failed" == apiResponseError.code && (result.a = null), apiResponseError;
        });
      };
      /**
       * @param {string} value
       * @param {!Object} code
       * @param {string} data
       * @return {?}
       */
      data.Ba = function(value, code, data) {
        if (!this.a) {
          this.a = postLink(reset(this));
        }
        var item = this;
        return this.a.then(function() {
          rewrite(code);
          finish(format(item.u, item.f, item.b, value, code, updatePresenterWindow(), data, item.c, void 0, item.h));
        }).m(function(apiResponseError) {
          throw "auth/network-request-failed" == apiResponseError.code && (item.a = null), apiResponseError;
        });
      };
      /**
       * @return {?}
       */
      data.ea = function() {
        var result = this;
        return message(this).then(function() {
          return result.j.ab;
        }).m(function() {
          throw result.a = null, new Buffer("network-request-failed");
        });
      };
      /**
       * @return {?}
       */
      data.Lb = function() {
        return true;
      };
      /**
       * @param {!Function} a
       * @return {undefined}
       */
      data.va = function(a) {
        this.g.push(a);
      };
      /**
       * @param {!Function} b
       * @return {undefined}
       */
      data.Ja = function(b) {
        s(this.g, function(a) {
          return a == b;
        });
      };
      /**
       * @param {string} a
       * @return {?}
       */
      (data = h.prototype).get = function(a) {
        return resolve(this.a.getItem(a)).then(function(action) {
          return action && dispatch(action);
        });
      };
      /**
       * @param {string} a
       * @param {!Object} text
       * @return {?}
       */
      data.set = function(a, text) {
        return resolve(this.a.setItem(a, parse(text)));
      };
      /**
       * @param {string} a
       * @return {?}
       */
      data.P = function(a) {
        return resolve(this.a.removeItem(a));
      };
      /**
       * @return {undefined}
       */
      data.Y = function() {
      };
      /**
       * @return {undefined}
       */
      data.aa = function() {
      };
      /**
       * @param {string} name
       * @param {!Object} type
       * @return {?}
       */
      (data = getOverlayStyles.prototype).set = function(name, type) {
        var x;
        /** @type {boolean} */
        var i = false;
        var fd = this;
        return defined(this).then(function(next) {
          return promisifyRequest((next = removeChild(setTimeout(x = next, true))).get(name));
        }).then(function(value) {
          var a = removeChild(setTimeout(x, true));
          return value ? (value.value = type, promisifyRequest(a.put(value))) : (fd.b++, i = true, (value = {}).fbase_key = name, value.value = type, promisifyRequest(a.add(value)));
        }).then(function() {
          /** @type {!Object} */
          fd.f[name] = type;
        }).ha(function() {
          if (i) {
            fd.b--;
          }
        });
      };
      /**
       * @param {string} name
       * @return {?}
       */
      data.get = function(name) {
        return defined(this).then(function(parentNode) {
          return promisifyRequest(removeChild(setTimeout(parentNode, false)).get(name));
        }).then(function(saveNum) {
          return saveNum && saveNum.value;
        });
      };
      /**
       * @param {string} k
       * @return {?}
       */
      data.P = function(k) {
        /** @type {boolean} */
        var e = false;
        var fileCoverage = this;
        return defined(this).then(function(parentNode) {
          return e = true, fileCoverage.b++, promisifyRequest(removeChild(setTimeout(parentNode, true)).delete(k));
        }).then(function() {
          delete fileCoverage.f[k];
        }).ha(function() {
          if (e) {
            fileCoverage.b--;
          }
        });
      };
      /**
       * @return {?}
       */
      data.Jc = function() {
        var q = this;
        return defined(this).then(function(t) {
          var objectStore = removeChild(setTimeout(t, false));
          return objectStore.getAll ? promisifyRequest(objectStore.getAll()) : new Promise(function(importedCB, callback) {
            /** @type {!Array} */
            var list = [];
            var idb_req = objectStore.openCursor();
            /**
             * @param {!Object} data
             * @return {undefined}
             */
            idb_req.onsuccess = function(data) {
              if (data = data.target.result) {
                list.push(data.value);
                data.continue();
              } else {
                importedCB(list);
              }
            };
            /**
             * @param {!Object} e
             * @return {undefined}
             */
            idb_req.onerror = function(e) {
              callback(Error(e.target.errorCode));
            };
          });
        }).then(function(in_tokens) {
          var func = {};
          /** @type {!Array} */
          var i = [];
          if (0 == q.b) {
            /** @type {number} */
            i = 0;
            for (; i < in_tokens.length; i++) {
              func[in_tokens[i].fbase_key] = in_tokens[i].value;
            }
            i = function check(data, config) {
              var i;
              /** @type {!Array} */
              var checkedList = [];
              for (i in data) {
                if (i in config) {
                  if (typeof data[i] != typeof config[i]) {
                    checkedList.push(i);
                  } else {
                    if (each(data[i])) {
                      t: {
                        var key = void 0;
                        var obj = data[i];
                        var source = config[i];
                        for (key in obj) {
                          if (!(key in source) || obj[key] !== source[key]) {
                            /** @type {boolean} */
                            key = false;
                            break t;
                          }
                        }
                        for (key in source) {
                          if (!(key in obj)) {
                            /** @type {boolean} */
                            key = false;
                            break t;
                          }
                        }
                        /** @type {boolean} */
                        key = true;
                      }
                      if (!key) {
                        checkedList.push(i);
                      }
                    } else {
                      if ("object" == typeof data[i] && null != data[i] && null != config[i]) {
                        if (0 < check(data[i], config[i]).length) {
                          checkedList.push(i);
                        }
                      } else {
                        if (data[i] !== config[i]) {
                          checkedList.push(i);
                        }
                      }
                    }
                  }
                } else {
                  checkedList.push(i);
                }
              }
              for (i in config) {
                if (!(i in data)) {
                  checkedList.push(i);
                }
              }
              return checkedList;
            }(q.f, func);
            q.f = func;
          }
          return i;
        });
      };
      /**
       * @param {!Function} d
       * @return {undefined}
       */
      data.Y = function(d) {
        if (0 == this.a.length) {
          (function(t) {
            if (t.c) {
              t.c.cancel("STOP_EVENT");
            }
            (function find() {
              t.c = validate(800).then(bind(t.Jc, t)).then(function(s) {
                if (0 < s.length) {
                  equal(t.a, function(errf) {
                    errf(s);
                  });
                }
              }).then(find).m(function(origErr) {
                if ("STOP_EVENT" != origErr.message) {
                  find();
                }
              });
              return t.c;
            })();
          })(this);
        }
        this.a.push(d);
      };
      /**
       * @param {?} date
       * @return {undefined}
       */
      data.aa = function(date) {
        s(this.a, function(i) {
          return i == date;
        });
        if (0 == this.a.length && this.c) {
          this.c.cancel("STOP_EVENT");
        }
      };
      /**
       * @param {string} name
       * @return {?}
       */
      (data = listener.prototype).get = function(name) {
        return this.b.then(function(propertyThresholdsMap) {
          return propertyThresholdsMap.get(name);
        });
      };
      /**
       * @param {string} type
       * @param {!Object} n
       * @return {?}
       */
      data.set = function(type, n) {
        return this.b.then(function(dirs) {
          return dirs.set(type, n);
        });
      };
      /**
       * @param {string} key
       * @return {?}
       */
      data.P = function(key) {
        return this.b.then(function(Keyboard) {
          return Keyboard.P(key);
        });
      };
      /**
       * @param {!Function} d
       * @return {undefined}
       */
      data.Y = function(d) {
        this.a.push(d);
      };
      /**
       * @param {?} date
       * @return {undefined}
       */
      data.aa = function(date) {
        s(this.a, function(i) {
          return i == date;
        });
      };
      /**
       * @param {string} name
       * @return {?}
       */
      (data = Converter.prototype).get = function(name) {
        return resolve(this.a[name]);
      };
      /**
       * @param {string} name
       * @param {!Object} obj
       * @return {?}
       */
      data.set = function(name, obj) {
        return this.a[name] = obj, resolve();
      };
      /**
       * @param {string} b
       * @return {?}
       */
      data.P = function(b) {
        return delete this.a[b], resolve();
      };
      /**
       * @return {undefined}
       */
      data.Y = function() {
      };
      /**
       * @return {undefined}
       */
      data.aa = function() {
      };
      /**
       * @param {string} name
       * @return {?}
       */
      (data = _init.prototype).get = function(name) {
        var self = this;
        return resolve().then(function() {
          return dispatch(self.a.getItem(name));
        });
      };
      /**
       * @param {string} key
       * @param {!Object} text
       * @return {?}
       */
      data.set = function(key, text) {
        var root = this;
        return resolve().then(function() {
          var group = parse(text);
          if (null === group) {
            root.P(key);
          } else {
            root.a.setItem(key, group);
          }
        });
      };
      /**
       * @param {string} value
       * @return {?}
       */
      data.P = function(value) {
        var app = this;
        return resolve().then(function() {
          app.a.removeItem(value);
        });
      };
      /**
       * @param {string} cb
       * @return {undefined}
       */
      data.Y = function(cb) {
        if (global.window) {
          map(global.window, "storage", cb);
        }
      };
      /**
       * @param {undefined} options
       * @return {undefined}
       */
      data.aa = function(options) {
        if (global.window) {
          add(global.window, "storage", options);
        }
      };
      /**
       * @return {?}
       */
      (data = effect.prototype).get = function() {
        return resolve(null);
      };
      /**
       * @return {?}
       */
      data.set = function() {
        return resolve();
      };
      /**
       * @return {?}
       */
      data.P = function() {
        return resolve();
      };
      /**
       * @return {undefined}
       */
      data.Y = function() {
      };
      /**
       * @return {undefined}
       */
      data.aa = function() {
      };
      /**
       * @param {string} name
       * @return {?}
       */
      (data = complete.prototype).get = function(name) {
        var self = this;
        return resolve().then(function() {
          return dispatch(self.a.getItem(name));
        });
      };
      /**
       * @param {string} key
       * @param {!Object} text
       * @return {?}
       */
      data.set = function(key, text) {
        var root = this;
        return resolve().then(function() {
          var group = parse(text);
          if (null === group) {
            root.P(key);
          } else {
            root.a.setItem(key, group);
          }
        });
      };
      /**
       * @param {string} value
       * @return {?}
       */
      data.P = function(value) {
        var app = this;
        return resolve().then(function() {
          app.a.removeItem(value);
        });
      };
      /**
       * @return {undefined}
       */
      data.Y = function() {
      };
      /**
       * @return {undefined}
       */
      data.aa = function() {
      };
      var tObj;
      var Ns;
      var mem = {
        w : _init,
        Pa : complete
      };
      var qr = {
        w : _init,
        Pa : complete
      };
      var rtn = {
        w : h,
        Pa : effect
      };
      var q = {
        w : _init,
        Pa : effect
      };
      var tmp = {
        Vc : "local",
        NONE : "none",
        Xc : "session"
      };
      /**
       * @param {!Object} input
       * @param {string} key
       * @return {?}
       */
      (data = _initialize.prototype).get = function(input, key) {
        return jQuery(this, input.w).get(trim(input, key));
      };
      /**
       * @param {!Object} a
       * @param {!Object} n
       * @param {string} start
       * @return {?}
       */
      data.set = function(a, n, start) {
        var name = trim(a, start);
        var r = this;
        var o = jQuery(this, a.w);
        return o.set(name, n).then(function() {
          return o.get(name);
        }).then(function(loadedPlugin) {
          if ("local" == a.w) {
            r.b[name] = loadedPlugin;
          }
        });
      };
      /**
       * @param {!Object} name
       * @param {string} _
       * @param {?} fn
       * @return {undefined}
       */
      data.addListener = function(name, _, fn) {
        name = trim(name, _);
        if (this.l) {
          this.b[name] = global.localStorage.getItem(name);
        }
        if (isMatch(this.a)) {
          jQuery(this, "local").Y(this.f);
          if (!(this.h || (retry() || !applyChange()) && global.indexedDB || !this.l)) {
            (function(obj) {
              getKey(obj);
              /** @type {number} */
              obj.c = setInterval(function() {
                var i;
                for (i in obj.a) {
                  var k = global.localStorage.getItem(i);
                  var id = obj.b[i];
                  if (k != id) {
                    obj.b[i] = k;
                    k = new start({
                      type : "storage",
                      key : i,
                      target : window,
                      oldValue : id,
                      newValue : k,
                      a : true
                    });
                    obj.Kb(k);
                  }
                }
              }, 1E3);
            })(this);
          }
        }
        if (!this.a[name]) {
          /** @type {!Array} */
          this.a[name] = [];
        }
        this.a[name].push(fn);
      };
      /**
       * @param {!Object} name
       * @param {string} _
       * @param {?} n
       * @return {undefined}
       */
      data.removeListener = function(name, _, n) {
        name = trim(name, _);
        if (this.a[name]) {
          s(this.a[name], function(special) {
            return special == n;
          });
          if (0 == this.a[name].length) {
            delete this.a[name];
          }
        }
        if (isMatch(this.a)) {
          jQuery(this, "local").aa(this.f);
          getKey(this);
        }
      };
      /**
       * @param {!Object} e
       * @return {undefined}
       */
      data.Kb = function(e) {
        if (e && e.f) {
          var i = e.a.key;
          if (null == i) {
            var b;
            for (b in this.a) {
              var data = this.b[b];
              if (void 0 === data) {
                /** @type {null} */
                data = null;
              }
              var i = global.localStorage.getItem(b);
              if (i !== data) {
                this.b[b] = i;
                this.Va(b);
              }
            }
          } else {
            if (0 == i.indexOf("firebase:") && this.a[i]) {
              if (void 0 !== e.a.a ? jQuery(this, "local").aa(this.f) : getKey(this), this.o) {
                if (b = global.localStorage.getItem(i), (data = e.a.newValue) !== b) {
                  if (null !== data) {
                    global.localStorage.setItem(i, data);
                  } else {
                    global.localStorage.removeItem(i);
                  }
                } else {
                  if (this.b[i] === data && void 0 === e.a.a) {
                    return;
                  }
                }
              }
              var a = this;
              /**
               * @return {undefined}
               */
              b = function() {
                if (!(void 0 === e.a.a && a.b[i] === global.localStorage.getItem(i))) {
                  a.b[i] = global.localStorage.getItem(i);
                  a.Va(i);
                }
              };
              if (window && element && 10 == element && global.localStorage.getItem(i) !== e.a.newValue && e.a.newValue !== e.a.oldValue) {
                setTimeout(b, 10);
              } else {
                b();
              }
            }
          }
        } else {
          equal(e, bind(this.Va, this));
        }
      };
      /**
       * @param {string} i
       * @return {undefined}
       */
      data.Va = function(i) {
        if (this.a[i]) {
          equal(this.a[i], function(saveNotifs) {
            saveNotifs();
          });
        }
      };
      var descname;
      var n = {
        name : "authEvent",
        w : "local"
      };
      expect(val, function() {
        /** @type {number} */
        this.b = -1;
      });
      /** @type {number} */
      var groupsize = 64;
      /** @type {number} */
      var dragstocreate = groupsize - 1;
      /** @type {!Array} */
      var l = [];
      /** @type {number} */
      var VIEW_COLUMN_VERSION = 0;
      for (; VIEW_COLUMN_VERSION < dragstocreate; VIEW_COLUMN_VERSION++) {
        /** @type {number} */
        l[VIEW_COLUMN_VERSION] = 0;
      }
      var res = F(128, l);
      /**
       * @return {undefined}
       */
      val.prototype.reset = function() {
        /** @type {number} */
        this.g = this.c = 0;
        this.a = global.Int32Array ? new Int32Array(this.h) : require(this.h);
      };
      /** @type {!Array} */
      var assetlaunched = [1116352408, 1899447441, 3049323471, 3921009573, 961987163, 1508970993, 2453635748, 2870763221, 3624381080, 310598401, 607225278, 1426881987, 1925078388, 2162078206, 2614888103, 3248222580, 3835390401, 4022224774, 264347078, 604807628, 770255983, 1249150122, 1555081692, 1996064986, 2554220882, 2821834349, 2952996808, 3210313671, 3336571891, 3584528711, 113926993, 338241895, 666307205, 773529912, 1294757372, 1396182291, 1695183700, 1986661051, 2177026350, 2456956037, 2730485921, 
      2820302411, 3259730800, 3345764771, 3516065817, 3600352804, 4094571909, 275423344, 430227734, 506948616, 659060556, 883997877, 958139571, 1322822218, 1537002063, 1747873779, 1955562222, 2024104815, 2227730452, 2361852424, 2428436474, 2756734187, 3204031479, 3329325298];
      expect(Image, val);
      /** @type {!Array} */
      var ctx = [1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225];
      /**
       * @return {?}
       */
      (data = r.prototype).ea = function() {
        return this.ya ? this.ya : this.ya = (resize(void 0) ? play().then(function() {
          return new Promise(function(saveNotifs, callback) {
            var doc = global.document;
            /** @type {number} */
            var autoResumeTimer = setTimeout(function() {
              callback(Error("Cordova framework is not ready."));
            }, 1E3);
            doc.addEventListener("deviceready", function() {
              clearTimeout(autoResumeTimer);
              saveNotifs();
            }, false);
          });
        }) : cb(Error("Cordova must run in an Android or iOS file scheme."))).then(function() {
          if ("function" != typeof stringify("universalLinks.subscribe", global)) {
            throw fsReadFile("cordova-universal-links-plugin is not installed");
          }
          if (void 0 === stringify("BuildInfo.packageName", global)) {
            throw fsReadFile("cordova-plugin-buildinfo is not installed");
          }
          if ("function" != typeof stringify("cordova.plugins.browsertab.openUrl", global)) {
            throw fsReadFile("cordova-plugin-browsertab is not installed");
          }
          if ("function" != typeof stringify("cordova.InAppBrowser.open", global)) {
            throw fsReadFile("cordova-plugin-inappbrowser is not installed");
          }
        }, function() {
          throw new Buffer("cordova-not-ready");
        });
      };
      /**
       * @param {!Object} options
       * @param {string} elem
       * @return {?}
       */
      data.Da = function(options, elem) {
        return elem(new Buffer("operation-not-supported-in-this-environment")), resolve();
      };
      /**
       * @return {?}
       */
      data.xb = function() {
        return cb(new Buffer("operation-not-supported-in-this-environment"));
      };
      /**
       * @return {?}
       */
      data.Lb = function() {
        return false;
      };
      /**
       * @return {?}
       */
      data.Gb = function() {
        return true;
      };
      /**
       * @return {?}
       */
      data.Bb = function() {
        return true;
      };
      /**
       * @param {string} value
       * @param {!Object} param
       * @param {!Object} num
       * @return {?}
       */
      data.Ba = function(value, param, num) {
        if (this.c) {
          return cb(new Buffer("redirect-operation-pending"));
        }
        var data = this;
        var r = global.document;
        /** @type {null} */
        var runner1 = null;
        /** @type {null} */
        var f = null;
        /** @type {null} */
        var cb = null;
        /** @type {null} */
        var c = null;
        return this.c = resolve().then(function() {
          return rewrite(param), handle(data);
        }).then(function() {
          return function(options, text, type, data) {
            var key = function() {
              /** @type {number} */
              var t = 20;
              /** @type {!Array} */
              var outChance = [];
              for (; 0 < t;) {
                outChance.push("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".charAt(Math.floor(62 * Math.random())));
                t--;
              }
              return outChance.join("");
            }();
            var node = new Node(text, data, null, key, new Buffer("no-auth-event"));
            var html = stringify("BuildInfo.packageName", global);
            if ("string" != typeof html) {
              throw new Buffer("invalid-cordova-configuration");
            }
            var content = stringify("BuildInfo.displayName", global);
            var opts = {};
            if (end().toLowerCase().match(/iphone|ipad|ipod/)) {
              /** @type {string} */
              opts.ibi = html;
            } else {
              if (!end().toLowerCase().match(/android/)) {
                return cb(new Buffer("operation-not-supported-in-this-environment"));
              }
              /** @type {string} */
              opts.apn = html;
            }
            if (content) {
              opts.appDisplayName = content;
            }
            key = tick(key);
            opts.sessionId = key;
            var result = format(options.u, options.j, options.l, text, type, null, data, options.o, opts, options.s);
            return options.ea().then(function() {
              var length = options.h;
              return options.v.a.set(n, node.C(), length);
            }).then(function() {
              var ret = stringify("cordova.plugins.browsertab.isAvailable", global);
              if ("function" != typeof ret) {
                throw new Buffer("invalid-cordova-configuration");
              }
              /** @type {null} */
              var require = null;
              ret(function(match) {
                if (match) {
                  if ("function" != typeof(require = stringify("cordova.plugins.browsertab.openUrl", global))) {
                    throw new Buffer("invalid-cordova-configuration");
                  }
                  require(result);
                } else {
                  if ("function" != typeof(require = stringify("cordova.InAppBrowser.open", global))) {
                    throw new Buffer("invalid-cordova-configuration");
                  }
                  /** @type {boolean} */
                  match = !(!(match = end()).match(/(iPad|iPhone|iPod).*OS 7_\d/i) && !match.match(/(iPad|iPhone|iPod).*OS 8_\d/i));
                  options.a = require(result, match ? "_blank" : "_system", "location=yes");
                }
              });
            });
          }(data, value, param, num);
        }).then(function() {
          return (new Promise(function(finalcb, callback) {
            /**
             * @return {?}
             */
            f = function() {
              var b = stringify("cordova.plugins.browsertab.close", global);
              return finalcb(), "function" == typeof b && b(), data.a && "function" == typeof data.a.close && (data.a.close(), data.a = null), false;
            };
            data.va(f);
            /**
             * @return {undefined}
             */
            cb = function() {
              if (!runner1) {
                runner1 = validate(2E3).then(function() {
                  callback(new Buffer("redirect-cancelled-by-user"));
                });
              }
            };
            /**
             * @return {undefined}
             */
            c = function() {
              if (setup()) {
                cb();
              }
            };
            r.addEventListener("resume", cb, false);
            if (!end().toLowerCase().match(/android/)) {
              r.addEventListener("visibilitychange", c, false);
            }
          })).m(function(canCreateDiscussions) {
            return draw(data).then(function() {
              throw canCreateDiscussions;
            });
          });
        }).ha(function() {
          if (cb) {
            r.removeEventListener("resume", cb, false);
          }
          if (c) {
            r.removeEventListener("visibilitychange", c, false);
          }
          if (runner1) {
            runner1.cancel();
          }
          if (f) {
            data.Ja(f);
          }
          /** @type {null} */
          data.c = null;
        });
      };
      /**
       * @param {!Function} p
       * @return {undefined}
       */
      data.va = function(p) {
        this.b.push(p);
        handle(this).m(function(a) {
          if ("auth/invalid-cordova-configuration" === a.code) {
            a = new Node("unknown", null, null, null, new Buffer("no-auth-event"));
            p(a);
          }
        });
      };
      /**
       * @param {!Function} b
       * @return {undefined}
       */
      data.Ja = function(b) {
        s(this.b, function(a) {
          return a == b;
        });
      };
      var candidate = {
        name : "pendingRedirect",
        w : "session"
      };
      /**
       * @return {undefined}
       */
      result.prototype.reset = function() {
        /** @type {boolean} */
        this.f = false;
        this.a.Ja(this.j);
        this.a = updateValues(this.v, this.l, this.u);
      };
      /**
       * @param {string} key
       * @return {undefined}
       */
      result.prototype.subscribe = function(key) {
        if (push(this.h, key) || this.h.push(key), !this.f) {
          var params = this;
          (function(t) {
            return t.b.get(candidate, t.a).then(function(current) {
              return "pending" == current;
            });
          })(this.g).then(function(canCreateDiscussions) {
            if (canCreateDiscussions) {
              currentStateIsSameWiki(params.g).then(function() {
                callbackFunction(params).m(function(date) {
                  var node = new Node("unknown", null, null, null, new Buffer("operation-not-supported-in-this-environment"));
                  if (toString(date)) {
                    params.o(node);
                  }
                });
              });
            } else {
              trigger(params);
            }
          }).m(function() {
            trigger(params);
          });
        }
      };
      /**
       * @param {?} stream
       * @return {undefined}
       */
      result.prototype.unsubscribe = function(stream) {
        s(this.h, function(url) {
          return url == stream;
        });
      };
      /**
       * @param {!Object} d
       * @return {?}
       */
      result.prototype.o = function(d) {
        if (!d) {
          throw new Buffer("invalid-auth-event");
        }
        /** @type {boolean} */
        var result = false;
        /** @type {number} */
        var id = 0;
        for (; id < this.h.length; id++) {
          var a = this.h[id];
          if (a.qb(d.b, d.c)) {
            if (result = this.b[d.b]) {
              result.h(d, a);
            }
            /** @type {boolean} */
            result = true;
            break;
          }
        }
        return isEmpty(this.c), result;
      };
      var model_instance = new link(2E3, 1E4);
      var relationControls = new link(3E4, 6E4);
      /**
       * @return {?}
       */
      result.prototype.da = function() {
        return this.c.da();
      };
      /**
       * @param {string} elem
       * @param {!Object} src
       * @param {string} num
       * @return {?}
       */
      result.prototype.Ba = function(elem, src, num) {
        var tRecord;
        var params = this;
        return function(t) {
          return t.b.set(candidate, "pending", t.a);
        }(this.g).then(function() {
          return params.a.Ba(elem, src, num).m(function(t) {
            if (toString(t)) {
              throw new Buffer("operation-not-supported-in-this-environment");
            }
            return tRecord = t, currentStateIsSameWiki(params.g).then(function() {
              throw tRecord;
            });
          }).then(function() {
            return params.a.Lb() ? new Promise(function() {
            }) : currentStateIsSameWiki(params.g).then(function() {
              return params.da();
            }).then(function() {
            }).m(function() {
            });
          });
        });
      };
      /**
       * @param {!Object} options
       * @param {string} elem
       * @param {!Object} p
       * @param {string} data
       * @return {?}
       */
      result.prototype.Da = function(options, elem, p, data) {
        return this.a.Da(p, function(url) {
          options.ga(elem, null, url, data);
        }, model_instance.get());
      };
      var events = {};
      /**
       * @return {undefined}
       */
      schedule.prototype.reset = function() {
        /** @type {null} */
        this.b = null;
        if (this.a) {
          this.a.cancel();
          /** @type {null} */
          this.a = null;
        }
      };
      /**
       * @param {!Object} e
       * @param {string} data
       * @return {undefined}
       */
      schedule.prototype.h = function(e, data) {
        if (e) {
          this.reset();
          /** @type {boolean} */
          this.g = true;
          var f = e.b;
          var center = e.c;
          var screenSmallerThanEditor = e.a && "auth/web-storage-unsupported" == e.a.code;
          var adjustHeight = e.a && "auth/operation-not-supported-in-this-environment" == e.a.code;
          if ("unknown" != f || screenSmallerThanEditor || adjustHeight) {
            if (e.a) {
              print(this, true, null, e.a);
              resolve();
            } else {
              if (data.wa(f, center)) {
                (function(s, o, t) {
                  t = t.wa(o.b, o.c);
                  var p = o.f;
                  var s = o.g;
                  /** @type {boolean} */
                  var event = !!o.b.match(/Redirect$/);
                  t(p, s).then(function(result) {
                    print(s, event, result, null);
                  }).m(function(separator) {
                    print(s, event, null, separator);
                  });
                })(this, e, data);
              } else {
                cb(new Buffer("invalid-auth-event"));
              }
            }
          } else {
            print(this, false, null, null);
            resolve();
          }
        } else {
          cb(new Buffer("invalid-auth-event"));
        }
      };
      /**
       * @return {?}
       */
      schedule.prototype.da = function() {
        var t = this;
        return new Promise(function(e, n) {
          if (t.b) {
            t.b().then(e, n);
          } else {
            t.f.push(e);
            t.c.push(n);
            (function(result) {
              var filename = new Buffer("timeout");
              if (result.a) {
                result.a.cancel();
              }
              result.a = validate(relationControls.get()).then(function() {
                if (!result.b) {
                  print(result, true, null, filename);
                }
              });
            })(t);
          }
        });
      };
      /**
       * @param {!Object} a
       * @param {string} o
       * @return {undefined}
       */
      Mix.prototype.h = function(a, o) {
        if (a) {
          var f = a.b;
          var l = a.c;
          if (a.a) {
            o.ga(a.b, null, a.a, a.c);
            resolve();
          } else {
            if (o.wa(f, l)) {
              (function(item, config) {
                var options = item.c;
                var b = item.b;
                config.wa(b, options)(item.f, item.g).then(function(key) {
                  config.ga(b, key, null, options);
                }).m(function(type) {
                  config.ga(b, null, type, options);
                });
              })(a, o);
            } else {
              cb(new Buffer("invalid-auth-event"));
            }
          }
        } else {
          cb(new Buffer("invalid-auth-event"));
        }
      };
      /**
       * @param {?} next
       * @return {?}
       */
      Server.prototype.confirm = function(next) {
        return next = navigate(this.verificationId, next), this.a(next);
      };
      /**
       * @return {undefined}
       */
      Range.prototype.start = function() {
        this.a = this.c;
        (function init(params, value) {
          params.stop();
          params.b = validate(function(o, y) {
            return y ? (o.a = o.c, o.g()) : (y = o.a, o.a *= 2, o.a > o.f && (o.a = o.f), y);
          }(params, value)).then(function() {
            return document = global.document, e = null, setup() || !document ? resolve() : (new Promise(function(C) {
              /**
               * @return {undefined}
               */
              e = function() {
                if (setup()) {
                  document.removeEventListener("visibilitychange", e, false);
                  C();
                }
              };
              document.addEventListener("visibilitychange", e, false);
            })).m(function(t) {
              throw document.removeEventListener("visibilitychange", e, false), t;
            });
            var document;
            var e;
          }).then(function() {
            return params.h();
          }).then(function() {
            init(params, true);
          }).m(function(d) {
            if (params.j(d)) {
              init(params, false);
            }
          });
        })(this, true);
      };
      /**
       * @return {undefined}
       */
      Range.prototype.stop = function() {
        if (this.b) {
          this.b.cancel();
          /** @type {null} */
          this.b = null;
        }
      };
      /**
       * @return {?}
       */
      _getMatrixAsString.prototype.C = function() {
        return {
          apiKey : this.f.b,
          refreshToken : this.a,
          accessToken : this.b,
          expirationTime : this.c
        };
      };
      /**
       * @param {boolean} ctx
       * @return {?}
       */
      _getMatrixAsString.prototype.getToken = function(ctx) {
        return ctx = !!ctx, this.b && !this.a ? cb(new Buffer("user-token-expired")) : ctx || !this.b || min() > this.c - 3E4 ? this.a ? getToken(this, {
          grant_type : "refresh_token",
          refresh_token : this.a
        }) : resolve(null) : resolve({
          accessToken : this.b,
          expirationTime : this.c,
          refreshToken : this.a
        });
      };
      /**
       * @return {?}
       */
      Matrix.prototype.C = function() {
        return {
          lastLoginAt : this.b,
          createdAt : this.a
        };
      };
      expect(Element, Event);
      expect(update, p);
      /**
       * @param {string} a
       * @return {undefined}
       */
      update.prototype.na = function(a) {
        /** @type {string} */
        this.ia = a;
        f3(this.b, a);
      };
      /**
       * @return {?}
       */
      update.prototype.ca = function() {
        return this.ia;
      };
      /**
       * @return {?}
       */
      update.prototype.Ka = function() {
        return require(this.O);
      };
      /**
       * @return {undefined}
       */
      update.prototype.Ga = function() {
        if (this.l.b) {
          this.l.stop();
          this.l.start();
        }
      };
      debug(update.prototype, "providerId", "firebase");
      /**
       * @return {?}
       */
      (data = update.prototype).reload = function() {
        var p = this;
        return call(this, reject(this).then(function() {
          return stat(p).then(function() {
            return list(p);
          }).then(id);
        }));
      };
      /**
       * @param {boolean} prop
       * @return {?}
       */
      data.ac = function(prop) {
        return this.F(prop).then(function(canCreateDiscussions) {
          return new function(t) {
            var config = ev(t);
            if (!(config && config.exp && config.auth_time && config.iat)) {
              throw new Buffer("internal-error", "An internal error occurred. The token obtained by Firebase appears to be malformed. Please retry the operation.");
            }
            done(this, {
              token : t,
              expirationTime : formatDate(1E3 * config.exp),
              authTime : formatDate(1E3 * config.auth_time),
              issuedAtTime : formatDate(1E3 * config.iat),
              signInProvider : config.firebase && config.firebase.sign_in_provider ? config.firebase.sign_in_provider : null,
              claims : config
            });
          }(canCreateDiscussions);
        });
      };
      /**
       * @param {boolean} c
       * @return {?}
       */
      data.F = function(c) {
        var self = this;
        return call(this, reject(this).then(function() {
          return self.h.getToken(c);
        }).then(function(options) {
          if (!options) {
            throw new Buffer("internal-error");
          }
          return options.accessToken != self.qa && (emit(self, options.accessToken), self.dispatchEvent(new Element("tokenChanged"))), insert(self, "refreshToken", options.refreshToken), options.accessToken;
        }));
      };
      /**
       * @param {boolean} start
       * @return {?}
       */
      data.getToken = function(start) {
        return Ei["firebase.User.prototype.getToken is deprecated. Please use firebase.User.prototype.getIdToken instead."] || (Ei["firebase.User.prototype.getToken is deprecated. Please use firebase.User.prototype.getIdToken instead."] = true, "undefined" != typeof console && "function" == typeof console.warn && console.warn("firebase.User.prototype.getToken is deprecated. Please use firebase.User.prototype.getIdToken instead.")), this.F(start);
      };
      /**
       * @param {!Object} user
       * @return {undefined}
       */
      data.uc = function(user) {
        if (!(user = user.users) || !user.length) {
          throw new Buffer("internal-error");
        }
        createUser(this, {
          uid : (user = user[0]).localId,
          displayName : user.displayName,
          photoURL : user.photoUrl,
          email : user.email,
          emailVerified : !!user.emailVerified,
          phoneNumber : user.phoneNumber,
          lastLoginAt : user.lastLoginAt,
          createdAt : user.createdAt
        });
        var data = function(value) {
          return (value = value.providerUserInfo) && value.length ? defer(value, function(providerInfo) {
            return new function(dogecoinAddress, loginId, email, name, canCreateDiscussions, phoneNumber) {
              done(this, {
                uid : dogecoinAddress,
                displayName : name || null,
                photoURL : canCreateDiscussions || null,
                email : email || null,
                phoneNumber : phoneNumber || null,
                providerId : loginId
              });
            }(providerInfo.rawId, providerInfo.providerId, providerInfo.email, providerInfo.displayName, providerInfo.photoUrl, providerInfo.phoneNumber);
          }) : [];
        }(user);
        /** @type {number} */
        var i = 0;
        for (; i < data.length; i++) {
          includes(this, data[i]);
        }
        insert(this, "isAnonymous", !(this.email && user.passwordHash || this.providerData && this.providerData.length));
      };
      /**
       * @param {?} b
       * @return {?}
       */
      data.bb = function(b) {
        var source = this;
        /** @type {null} */
        var args = null;
        return call(this, b.f(this.b, this.uid).then(function(callback) {
          return clone(source, callback), args = toArray(source, callback, "reauthenticate"), source.j = null, source.reload();
        }).then(function() {
          return args;
        }), true);
      };
      /**
       * @param {?} conf
       * @return {?}
       */
      data.vc = function(conf) {
        return this.bb(conf).then(function() {
        });
      };
      /**
       * @param {!Object} a
       * @return {?}
       */
      data.$a = function(a) {
        var data = this;
        /** @type {null} */
        var result = null;
        return call(this, put(this, a.providerId).then(function() {
          return data.F();
        }).then(function(value) {
          return a.c(data.b, value);
        }).then(function(bar) {
          return result = toArray(data, bar, "link"), all(data, bar);
        }).then(function() {
          return result;
        }));
      };
      /**
       * @param {!Object} x
       * @return {?}
       */
      data.mc = function(x) {
        return this.$a(x).then(function(LoginModel) {
          return LoginModel.user;
        });
      };
      /**
       * @param {string} name
       * @param {!Object} value
       * @return {?}
       */
      data.nc = function(name, value) {
        var self = this;
        return call(this, put(this, "phone").then(function() {
          return when(use(self), name, value, bind(self.$a, self));
        }));
      };
      /**
       * @param {string} n
       * @param {!Object} v
       * @return {?}
       */
      data.wc = function(n, v) {
        var self = this;
        return call(this, resolve().then(function() {
          return when(use(self), n, v, bind(self.bb, self));
        }), true);
      };
      /**
       * @param {string} key
       * @return {?}
       */
      data.mb = function(key) {
        var map = this;
        return call(this, this.F().then(function(size) {
          return map.b.mb(size, key);
        }).then(function(deep) {
          return clone(map, deep), map.reload();
        }));
      };
      /**
       * @param {!Object} t
       * @return {?}
       */
      data.Nc = function(t) {
        var node = this;
        return call(this, this.F().then(function(value) {
          return t.c(node.b, value);
        }).then(function(deep) {
          return clone(node, deep), node.reload();
        }));
      };
      /**
       * @param {!Object} o
       * @return {?}
       */
      data.nb = function(o) {
        var e = this;
        return call(this, this.F().then(function(iterator) {
          return e.b.nb(iterator, o);
        }).then(function(deep) {
          return clone(e, deep), e.reload();
        }));
      };
      /**
       * @param {!Object} user
       * @return {?}
       */
      data.ob = function(user) {
        if (void 0 === user.displayName && void 0 === user.photoURL) {
          return reject(this);
        }
        var i = this;
        return call(this, this.F().then(function(n) {
          return i.b.ob(n, {
            displayName : user.displayName,
            photoUrl : user.photoURL
          });
        }).then(function(data) {
          return clone(i, data), insert(i, "displayName", data.displayName || null), insert(i, "photoURL", data.photoUrl || null), equal(i.providerData, function(event) {
            if ("password" === event.providerId) {
              debug(event, "displayName", i.displayName);
              debug(event, "photoURL", i.photoURL);
            }
          }), list(i);
        }).then(id));
      };
      /**
       * @param {string} O
       * @return {?}
       */
      data.Mc = function(O) {
        var a = this;
        return call(this, stat(this).then(function(element) {
          return push(getPath(a), O) ? function(t, id, n) {
            return callback(t, service, {
              idToken : id,
              deleteProvider : n
            });
          }(a.b, element, [O]).then(function(response) {
            var map = {};
            return equal(response.providerUserInfo || [], function(event) {
              /** @type {boolean} */
              map[event.providerId] = true;
            }), equal(getPath(a), function(transition) {
              if (!map[transition]) {
                contains(a, transition);
              }
            }), map[d.PROVIDER_ID] || debug(a, "phoneNumber", null), list(a);
          }) : list(a).then(function() {
            throw new Buffer("no-such-provider");
          });
        }));
      };
      /**
       * @return {?}
       */
      data.delete = function() {
        var data = this;
        return call(this, this.F().then(function(token) {
          return callback(data.b, table, {
            idToken : token
          });
        }).then(function() {
          data.dispatchEvent(new Element("userDeleted"));
        })).then(function() {
          /** @type {number} */
          var i = 0;
          for (; i < data.D.length; i++) {
            data.D[i].cancel("app-deleted");
          }
          logger(data, null);
          store(data, null);
          /** @type {!Array} */
          data.D = [];
          /** @type {boolean} */
          data.o = true;
          openModal(data);
          debug(data, "refreshToken", null);
          if (data.a) {
            data.a.unsubscribe(data);
          }
        });
      };
      /**
       * @param {string} minMajor
       * @param {?} minMinor
       * @return {?}
       */
      data.qb = function(minMajor, minMinor) {
        return !!("linkViaPopup" == minMajor && (this.g || null) == minMinor && this.f || "reauthViaPopup" == minMajor && (this.g || null) == minMinor && this.f || "linkViaRedirect" == minMajor && (this.$ || null) == minMinor || "reauthViaRedirect" == minMajor && (this.$ || null) == minMinor);
      };
      /**
       * @param {string} type
       * @param {!Object} x
       * @param {!Object} t
       * @param {string} data
       * @return {undefined}
       */
      data.ga = function(type, x, t, data) {
        if (!("linkViaPopup" != type && "reauthViaPopup" != type || data != (this.g || null))) {
          if (t && this.v) {
            this.v(t);
          } else {
            if (x && !t && this.f) {
              this.f(x);
            }
          }
          if (this.c) {
            this.c.cancel();
            /** @type {null} */
            this.c = null;
          }
          delete this.f;
          delete this.v;
        }
      };
      /**
       * @param {string} a
       * @param {string} b
       * @return {?}
       */
      data.wa = function(a, b) {
        return "linkViaPopup" == a && b == (this.g || null) ? bind(this.vb, this) : "reauthViaPopup" == a && b == (this.g || null) ? bind(this.wb, this) : "linkViaRedirect" == a && (this.$ || null) == b ? bind(this.vb, this) : "reauthViaRedirect" == a && (this.$ || null) == b ? bind(this.wb, this) : null;
      };
      /**
       * @param {!Object} a
       * @return {?}
       */
      data.oc = function(a) {
        var options = this;
        return remove(this, "linkViaPopup", a, function() {
          return put(options, a.providerId).then(function() {
            return list(options);
          });
        }, false);
      };
      /**
       * @param {!Object} _node
       * @return {?}
       */
      data.xc = function(_node) {
        return remove(this, "reauthViaPopup", _node, function() {
          return resolve();
        }, true);
      };
      /**
       * @param {!Object} obj
       * @return {?}
       */
      data.pc = function(obj) {
        var targetNode = this;
        return set(this, "linkViaRedirect", obj, function() {
          return put(targetNode, obj.providerId);
        }, false);
      };
      /**
       * @param {!Object} propVal
       * @return {?}
       */
      data.yc = function(propVal) {
        return set(this, "reauthViaRedirect", propVal, function() {
          return resolve();
        }, true);
      };
      /**
       * @param {string} baseUri
       * @param {string} options
       * @return {?}
       */
      data.vb = function(baseUri, options) {
        var obj = this;
        if (this.c) {
          this.c.cancel();
          /** @type {null} */
          this.c = null;
        }
        /** @type {null} */
        var result = null;
        return call(this, this.F().then(function(token) {
          return children(obj.b, {
            requestUri : baseUri,
            sessionId : options,
            idToken : token
          });
        }).then(function(bar) {
          return result = toArray(obj, bar, "link"), all(obj, bar);
        }).then(function() {
          return result;
        }));
      };
      /**
       * @param {string} baseUri
       * @param {string} options
       * @return {?}
       */
      data.wb = function(baseUri, options) {
        var object = this;
        if (this.c) {
          this.c.cancel();
          /** @type {null} */
          this.c = null;
        }
        /** @type {null} */
        var properties = null;
        return call(this, resolve().then(function() {
          return t(Collection(object.b, {
            requestUri : baseUri,
            sessionId : options
          }), object.uid);
        }).then(function(data) {
          return properties = toArray(object, data, "reauthenticate"), clone(object, data), object.j = null, object.reload();
        }).then(function() {
          return properties;
        }), true);
      };
      /**
       * @param {?} name
       * @return {?}
       */
      data.fb = function(name) {
        var obj = this;
        /** @type {null} */
        var timemodified = null;
        return call(this, this.F().then(function(timemod) {
          return timemodified = timemod, void 0 === name || isMatch(name) ? {} : getComputedColor(new Query(name));
        }).then(function(keyReads) {
          return obj.b.fb(timemodified, keyReads);
        }).then(function(Constructor) {
          if (obj.email != Constructor) {
            return obj.reload();
          }
        }).then(function() {
        }));
      };
      /**
       * @return {?}
       */
      data.toJSON = function() {
        return this.C();
      };
      /**
       * @return {?}
       */
      data.C = function() {
        var data = {
          uid : this.uid,
          displayName : this.displayName,
          photoURL : this.photoURL,
          email : this.email,
          emailVerified : this.emailVerified,
          phoneNumber : this.phoneNumber,
          isAnonymous : this.isAnonymous,
          providerData : [],
          apiKey : this.G,
          appName : this.s,
          authDomain : this.B,
          stsTokenManager : this.h.C(),
          redirectEventId : this.$ || null
        };
        return this.metadata && log(data, this.metadata.C()), equal(this.providerData, function(e) {
          data.providerData.push(function(t) {
            var e;
            var iface = {};
            for (e in t) {
              if (t.hasOwnProperty(e)) {
                iface[e] = t[e];
              }
            }
            return iface;
          }(e));
        }), data;
      };
      var foo = {
        name : "redirectUser",
        w : "session"
      };
      /**
       * @return {undefined}
       */
      connect.prototype.g = function() {
        var o = this;
        var a = parseFloat("local");
        encrypt(this, function() {
          return resolve().then(function() {
            return o.c && "local" != o.c.w ? o.b.get(a, o.a) : null;
          }).then(function(n) {
            if (n) {
              return u(o, "local").then(function() {
                o.c = a;
              });
            }
          });
        });
      };
      var args = {
        name : "persistence",
        w : "session"
      };
      /**
       * @param {string} n
       * @return {?}
       */
      connect.prototype.ib = function(n) {
        /** @type {null} */
        var e = null;
        var node = this;
        return function(html) {
          var InventoryBuffer = new Buffer("invalid-persistence-type");
          var buff0 = new Buffer("unsupported-persistence-type");
          t: {
            for (contentType in tmp) {
              if (tmp[contentType] == html) {
                /** @type {boolean} */
                var contentType = true;
                break t;
              }
            }
            /** @type {boolean} */
            contentType = false;
          }
          if (!contentType || "string" != typeof html) {
            throw InventoryBuffer;
          }
          switch(css()) {
            case "ReactNative":
              if ("session" === html) {
                throw buff0;
              }
              break;
            case "Node":
              if ("none" !== html) {
                throw buff0;
              }
              break;
            default:
              if (!detectPrivateMode() && "none" !== html) {
                throw buff0;
              }
          }
        }(n), encrypt(this, function() {
          return n != node.c.w ? node.b.get(node.c, node.a).then(function(tmp) {
            return e = tmp, u(node, n);
          }).then(function() {
            if (node.c = parseFloat(n), e) {
              return node.b.set(node.c, e, node.a);
            }
          }) : resolve();
        });
      };
      expect(init, p);
      expect(_Event, Event);
      expect(ProgressEvent, Event);
      /**
       * @param {string} node
       * @return {?}
       */
      (data = init.prototype).ib = function(node) {
        return request(this, node = this.h.ib(node));
      };
      /**
       * @param {number} w
       * @return {undefined}
       */
      data.na = function(w) {
        if (!(this.W === w || this.l)) {
          /** @type {number} */
          this.W = w;
          f3(this.b, this.W);
          this.dispatchEvent(new _Event(this.ca()));
        }
      };
      /**
       * @return {?}
       */
      data.ca = function() {
        return this.W;
      };
      /**
       * @return {undefined}
       */
      data.Oc = function() {
        var nav = global.navigator;
        this.na(nav && (nav.languages && nav.languages[0] || nav.language || nav.userLanguage) || null);
      };
      /**
       * @param {?} a
       * @return {undefined}
       */
      data.qc = function(a) {
        this.D.push(a);
        reply(this.b, config.SDK_VERSION ? success(config.SDK_VERSION, this.D) : null);
        this.dispatchEvent(new ProgressEvent(this.D));
      };
      /**
       * @return {?}
       */
      data.Ka = function() {
        return require(this.D);
      };
      /**
       * @return {?}
       */
      data.toJSON = function() {
        return {
          apiKey : build(this).options.apiKey,
          authDomain : build(this).options.authDomain,
          appName : build(this).name,
          currentUser : $(this) && $(this).C()
        };
      };
      /**
       * @param {?} vEventName
       * @param {?} vEventData
       * @return {?}
       */
      data.qb = function(vEventName, vEventData) {
        switch(vEventName) {
          case "unknown":
          case "signInViaRedirect":
            return true;
          case "signInViaPopup":
            return this.g == vEventData && !!this.f;
          default:
            return false;
        }
      };
      /**
       * @param {string} b
       * @param {!Object} x
       * @param {!Object} c
       * @param {string} a
       * @return {undefined}
       */
      data.ga = function(b, x, c, a) {
        if ("signInViaPopup" == b && this.g == a) {
          if (c && this.v) {
            this.v(c);
          } else {
            if (x && !c && this.f) {
              this.f(x);
            }
          }
          if (this.c) {
            this.c.cancel();
            /** @type {null} */
            this.c = null;
          }
          delete this.f;
          delete this.v;
        }
      };
      /**
       * @param {string} b
       * @param {?} c
       * @return {?}
       */
      data.wa = function(b, c) {
        return "signInViaRedirect" == b || "signInViaPopup" == b && this.g == c && this.f ? bind(this.Xb, this) : null;
      };
      /**
       * @param {string} endpoint
       * @param {string} channel
       * @return {?}
       */
      data.Xb = function(endpoint, channel) {
        var result = this;
        endpoint = {
          requestUri : endpoint,
          sessionId : channel
        };
        if (this.c) {
          this.c.cancel();
          /** @type {null} */
          this.c = null;
        }
        /** @type {null} */
        var n = null;
        /** @type {null} */
        var json = null;
        var o = getApi(result.b, endpoint).then(function(data) {
          return n = indexOf(data), json = pick(data), data;
        });
        return request(this, endpoint = result.V.then(function() {
          return o;
        }).then(function(val) {
          return generate(result, val);
        }).then(function() {
          return err({
            user : $(result),
            credential : n,
            additionalUserInfo : json,
            operationType : "signIn"
          });
        }));
      };
      /**
       * @param {!Object} msg
       * @return {?}
       */
      data.Gc = function(msg) {
        if (!size()) {
          return cb(new Buffer("operation-not-supported-in-this-environment"));
        }
        var self = this;
        var data = info(msg.providerId);
        var t = removeItem();
        /** @type {null} */
        var err = null;
        if ((!query() || click()) && build(this).options.authDomain && msg.isOAuthProvider) {
          err = format(build(this).options.authDomain, build(this).options.apiKey, build(this).name, "signInViaPopup", msg, null, t, config.SDK_VERSION || null);
        }
        var params = render(err, data && data.Aa, data && data.za);
        return request(this, data = saveFile(this).then(function(i) {
          return x(i, params, "signInViaPopup", msg, t, !!err);
        }).then(function() {
          return new Promise(function(i, elem) {
            self.ga("signInViaPopup", null, new Buffer("cancelled-popup-request"), self.g);
            /** @type {!Function} */
            self.f = i;
            /** @type {number} */
            self.v = elem;
            self.g = t;
            self.c = self.a.Da(self, "signInViaPopup", params, t);
          });
        }).then(function(e) {
          return params && isObject(params), e ? err(e) : null;
        }).m(function(uploadFileTpl) {
          throw params && isObject(params), uploadFileTpl;
        }));
      };
      /**
       * @param {!Object} t
       * @return {?}
       */
      data.Hc = function(t) {
        if (!size()) {
          return cb(new Buffer("operation-not-supported-in-this-environment"));
        }
        var newV = this;
        return request(this, saveFile(this).then(function() {
          return encrypt(v = newV.h, function() {
            return v.b.set(args, v.c.w, v.a);
          });
          var v;
        }).then(function() {
          return newV.a.Ba("signInViaRedirect", t);
        }));
      };
      /**
       * @return {?}
       */
      data.da = function() {
        if (!size()) {
          return cb(new Buffer("operation-not-supported-in-this-environment"));
        }
        var datepicker = this;
        return request(this, saveFile(this).then(function() {
          return datepicker.a.da();
        }).then(function(e) {
          return e ? err(e) : null;
        }));
      };
      /**
       * @return {?}
       */
      data.kb = function() {
        var data = this;
        return request(this, this.j.then(function() {
          return $(data) ? (select(data, null), format_time(data.h).then(function() {
            formatResult(data);
          })) : resolve();
        }));
      };
      /**
       * @return {?}
       */
      data.Ic = function() {
        var query = this;
        return decrypt(this.h, build(this).options.authDomain).then(function(data) {
          if (!query.l) {
            var value;
            if (value = $(query) && data) {
              value = $(query).uid;
              var id = data.uid;
              /** @type {boolean} */
              value = void 0 !== value && null !== value && "" !== value && void 0 !== id && null !== id && "" !== id && value == id;
            }
            if (value) {
              return create($(query), data), $(query).F();
            }
            if ($(query) || data) {
              select(query, data);
              if (data) {
                getTitle(data);
                data.fa = query.G;
              }
              if (query.a) {
                query.a.subscribe(query);
              }
              formatResult(query);
            }
          }
        });
      };
      /**
       * @param {!Object} key
       * @return {?}
       */
      data.ka = function(key) {
        return C(this.h, key);
      };
      /**
       * @return {undefined}
       */
      data.Yb = function() {
        formatResult(this);
        this.ka($(this));
      };
      /**
       * @return {undefined}
       */
      data.gc = function() {
        this.kb();
      };
      /**
       * @return {undefined}
       */
      data.hc = function() {
        this.kb();
      };
      /**
       * @param {!Node} f
       * @return {undefined}
       */
      data.ic = function(f) {
        var value = this;
        this.addAuthTokenListener(function() {
          f.next($(value));
        });
      };
      /**
       * @param {!Node} dropDown
       * @return {undefined}
       */
      data.jc = function(dropDown) {
        var value = this;
        !function(data, res) {
          data.I.push(res);
          request(data, data.j.then(function() {
            if (!data.l && push(data.I, res) && data.O !== data.getUid()) {
              data.O = data.getUid();
              res(lz(data));
            }
          }));
        }(this, function() {
          dropDown.next($(value));
        });
      };
      /**
       * @param {!Object} callback
       * @param {?} id
       * @param {?} env
       * @return {?}
       */
      data.sc = function(callback, id, env) {
        var value = this;
        return this.X && config.Promise.resolve().then(function() {
          if (isNaN(callback)) {
            callback($(value));
          } else {
            if (isNaN(callback.next)) {
              callback.next($(value));
            }
          }
        }), this.Ob(callback, id, env);
      };
      /**
       * @param {!Object} options
       * @param {?} defaults
       * @param {?} args
       * @return {?}
       */
      data.rc = function(options, defaults, args) {
        var item = this;
        return this.X && config.Promise.resolve().then(function() {
          item.O = item.getUid();
          if (isNaN(options)) {
            options($(item));
          } else {
            if (isNaN(options.next)) {
              options.next($(item));
            }
          }
        }), this.Pb(options, defaults, args);
      };
      /**
       * @param {boolean} s
       * @return {?}
       */
      data.$b = function(s) {
        var endColorCoords = this;
        return request(this, this.j.then(function() {
          return $(endColorCoords) ? $(endColorCoords).F(s).then(function(accessToken) {
            return {
              accessToken : accessToken
            };
          }) : null;
        }));
      };
      /**
       * @param {string} name
       * @return {?}
       */
      data.Cc = function(name) {
        return this.Hb(name).then(function(LoginModel) {
          return LoginModel.user;
        });
      };
      /**
       * @param {string} expected
       * @return {?}
       */
      data.Hb = function(expected) {
        var node = this;
        return this.j.then(function() {
          return find(node, callback(node.b, state, {
            token : expected
          }));
        }).then(function(alarm) {
          var data = alarm.user;
          return insert(data, "isAnonymous", false), node.ka(data), alarm;
        });
      };
      /**
       * @param {string} email
       * @param {!Object} password1
       * @return {?}
       */
      data.Ib = function(email, password1) {
        var typedData = this;
        return this.j.then(function() {
          return find(typedData, callback(typedData.b, found, {
            email : email,
            password : password1
          }));
        });
      };
      /**
       * @param {string} comment
       * @param {!Object} password1
       * @return {?}
       */
      data.Dc = function(comment, password1) {
        return this.Ib(comment, password1).then(function(LoginModel) {
          return LoginModel.user;
        });
      };
      /**
       * @param {string} n
       * @param {!Object} messageId
       * @return {?}
       */
      data.Sb = function(n, messageId) {
        return this.sb(n, messageId).then(function(LoginModel) {
          return LoginModel.user;
        });
      };
      /**
       * @param {string} to
       * @param {!Object} id
       * @return {?}
       */
      data.sb = function(to, id) {
        var typedData = this;
        return this.j.then(function() {
          return find(typedData, callback(typedData.b, that, {
            email : to,
            password : id
          }));
        });
      };
      /**
       * @param {?} data
       * @return {?}
       */
      data.Bc = function(data) {
        return this.Oa(data).then(function(LoginModel) {
          return LoginModel.user;
        });
      };
      /**
       * @param {?} d
       * @return {?}
       */
      data.Oa = function(d) {
        var value = this;
        return this.j.then(function() {
          return find(value, d.xa(value.b));
        });
      };
      /**
       * @return {?}
       */
      data.jb = function() {
        return this.Jb().then(function(LoginModel) {
          return LoginModel.user;
        });
      };
      /**
       * @return {?}
       */
      data.Jb = function() {
        var node = this;
        return this.j.then(function() {
          var result = $(node);
          return result && result.isAnonymous ? err({
            user : result,
            credential : null,
            additionalUserInfo : err({
              providerId : null,
              isNewUser : false
            }),
            operationType : "signIn"
          }) : find(node, node.b.jb()).then(function(htmlAndUser) {
            var user = htmlAndUser.user;
            return insert(user, "isAnonymous", true), node.ka(user), htmlAndUser;
          });
        });
      };
      /**
       * @return {?}
       */
      data.getUid = function() {
        return $(this) && $(this).uid || null;
      };
      /**
       * @param {string} unhighlightedCode
       * @return {undefined}
       */
      data.Qb = function(unhighlightedCode) {
        this.addAuthTokenListener(unhighlightedCode);
        this.s++;
        if (0 < this.s && $(this)) {
          processTemplate($(this));
        }
      };
      /**
       * @param {?} move
       * @return {undefined}
       */
      data.zc = function(move) {
        var oldSubs = this;
        equal(this.o, function(options) {
          if (options == move) {
            oldSubs.s--;
          }
        });
        if (0 > this.s) {
          /** @type {number} */
          this.s = 0;
        }
        if (0 == this.s && $(this)) {
          openModal($(this));
        }
        this.removeAuthTokenListener(move);
      };
      /**
       * @param {string} text
       * @return {undefined}
       */
      data.addAuthTokenListener = function(text) {
        var m = this;
        this.o.push(text);
        request(this, this.j.then(function() {
          if (!m.l) {
            if (push(m.o, text)) {
              text(lz(m));
            }
          }
        }));
      };
      /**
       * @param {?} listener
       * @return {undefined}
       */
      data.removeAuthTokenListener = function(listener) {
        s(this.o, function(l) {
          return l == listener;
        });
      };
      /**
       * @return {?}
       */
      data.delete = function() {
        /** @type {boolean} */
        this.l = true;
        /** @type {number} */
        var t = 0;
        for (; t < this.N.length; t++) {
          this.N[t].cancel("app-deleted");
        }
        return this.N = [], this.h && (t = this.h).b.removeListener(parseFloat("local"), t.a, this.ia), this.a && this.a.unsubscribe(this), config.Promise.resolve();
      };
      /**
       * @param {string} primaryTxHex
       * @return {?}
       */
      data.Vb = function(primaryTxHex) {
        return request(this, function(t, primaryTxHex) {
          return callback(t, properties, {
            identifier : primaryTxHex,
            continueUri : migrateWarn() ? updatePresenterWindow() : "http://localhost"
          }).then(function(canCreateDiscussions) {
            return canCreateDiscussions.allProviders || [];
          });
        }(this.b, primaryTxHex));
      };
      /**
       * @param {string} primaryTxHex
       * @return {?}
       */
      data.Wb = function(primaryTxHex) {
        return request(this, function(t, primaryTxHex) {
          return callback(t, properties, {
            identifier : primaryTxHex,
            continueUri : migrateWarn() ? updatePresenterWindow() : "http://localhost"
          }).then(function(canCreateDiscussions) {
            return canCreateDiscussions.signinMethods || [];
          });
        }(this.b, primaryTxHex));
      };
      /**
       * @param {string} e
       * @return {?}
       */
      data.kc = function(e) {
        return !!walk(e);
      };
      /**
       * @param {!Object} id
       * @param {?} callback
       * @return {?}
       */
      data.hb = function(id, callback) {
        var tParentMatrix = this;
        return request(this, resolve().then(function() {
          var cmd = new Query(callback);
          if (!cmd.c) {
            throw new Buffer("argument-error", aKey + " must be true when sending sign in link to email");
          }
          return getComputedColor(cmd);
        }).then(function(e) {
          return tParentMatrix.b.hb(id, e);
        }).then(function() {
        }));
      };
      /**
       * @param {!Function} time
       * @return {?}
       */
      data.Pc = function(time) {
        return this.Ia(time).then(function(results) {
          return results.data.email;
        });
      };
      /**
       * @param {?} time
       * @param {string} delta
       * @return {?}
       */
      data.Wa = function(time, delta) {
        return request(this, this.b.Wa(time, delta).then(function() {
        }));
      };
      /**
       * @param {!Function} time
       * @return {?}
       */
      data.Ia = function(time) {
        return request(this, this.b.Ia(time).then(function(canCreateDiscussions) {
          return new function(t) {
            var data = {};
            var name = t[mode];
            var value = t[locale];
            if (!(t = t[codePoint]) || t != string && !name) {
              throw Error("Invalid provider user info!");
            }
            data[childName] = value || null;
            data[nameKey] = name || null;
            debug(this, provider, t);
            debug(this, storageName, _get(data));
          }(canCreateDiscussions);
        }));
      };
      /**
       * @param {?} time
       * @return {?}
       */
      data.Ua = function(time) {
        return request(this, this.b.Ua(time).then(function() {
        }));
      };
      /**
       * @param {!Object} ast
       * @param {?} name
       * @return {?}
       */
      data.gb = function(ast, name) {
        var keymap = this;
        return request(this, resolve().then(function() {
          return void 0 === name || isMatch(name) ? {} : getComputedColor(new Query(name));
        }).then(function(a) {
          return keymap.b.gb(ast, a);
        }).then(function() {
        }));
      };
      /**
       * @param {string} name
       * @param {!Object} cb
       * @return {?}
       */
      data.Fc = function(name, cb) {
        return request(this, when(this, name, cb, bind(this.Oa, this)));
      };
      /**
       * @param {string} data
       * @param {?} err
       * @return {?}
       */
      data.Ec = function(data, err) {
        var other_list = this;
        return request(this, resolve().then(function() {
          var d = cleanup(data, err || updatePresenterWindow());
          return other_list.Oa(d);
        }));
      };
      /** @type {string} */
      var propertyName = "callback";
      /** @type {string} */
      var id_item = "expired-callback";
      /** @type {string} */
      var index = "sitekey";
      /** @type {string} */
      var SIZEFIELD = "size";
      /**
       * @return {?}
       */
      (data = run.prototype).ya = function() {
        var values = this;
        return this.c ? this.c : this.c = verify(this, resolve().then(function() {
          if (migrateWarn() && !replace()) {
            return play();
          }
          throw new Buffer("operation-not-supported-in-this-environment", "RecaptchaVerifier is only supported in a browser HTTP/HTTPS environment.");
        }).then(function() {
          return function(obj, version) {
            return new Promise(function(saveNotifs, callback) {
              /** @type {number} */
              var autoResumeTimer = setTimeout(function() {
                callback(new Buffer("network-request-failed"));
              }, clickedPlaylist.get());
              if (!global.grecaptcha || version !== obj.c && !obj.b) {
                /**
                 * @return {undefined}
                 */
                global[obj.a] = function() {
                  if (global.grecaptcha) {
                    /** @type {string} */
                    obj.c = version;
                    var resolve = global.grecaptcha.render;
                    /**
                     * @param {?} expr
                     * @param {?} context
                     * @return {?}
                     */
                    global.grecaptcha.render = function(expr, context) {
                      return expr = resolve(expr, context), obj.b++, expr;
                    };
                    clearTimeout(autoResumeTimer);
                    saveNotifs();
                  } else {
                    clearTimeout(autoResumeTimer);
                    callback(new Buffer("internal-error"));
                  }
                  delete global[obj.a];
                };
                var result = check(container, {
                  onload : obj.a,
                  hl : version || ""
                });
                resolve(fetch(result)).m(function() {
                  clearTimeout(autoResumeTimer);
                  callback(new Buffer("internal-error", "Unable to load external reCAPTCHA dependencies!"));
                });
              } else {
                clearTimeout(autoResumeTimer);
                saveNotifs();
              }
            });
          }(splice(), values.s());
        }).then(function() {
          return callback(values.u, user, {});
        }).then(function(patchElement) {
          values.a[index] = patchElement.recaptchaSiteKey;
        }).m(function(canCreateDiscussions) {
          throw values.c = null, canCreateDiscussions;
        }));
      };
      /**
       * @return {?}
       */
      data.render = function() {
        save(this);
        var me = this;
        return verify(this, this.ya().then(function() {
          if (null === me.b) {
            var el = me.l;
            if (!me.h) {
              var outerLayer = removeEventListener(el);
              el = createElement("DIV");
              outerLayer.appendChild(el);
            }
            me.b = grecaptcha.render(el, me.a);
          }
          return me.b;
        }));
      };
      /**
       * @return {?}
       */
      data.verify = function() {
        save(this);
        var t = this;
        return verify(this, this.render().then(function(widgetId) {
          return new Promise(function(n) {
            var ERR_INVALID_AUTH = grecaptcha.getResponse(widgetId);
            if (ERR_INVALID_AUTH) {
              n(ERR_INVALID_AUTH);
            } else {
              /**
               * @param {?} a
               * @return {undefined}
               */
              var i = function(a) {
                if (a) {
                  (function(a, all) {
                    s(a.j, function(letter) {
                      return letter == all;
                    });
                  })(t, i);
                  n(a);
                }
              };
              t.j.push(i);
              if (t.h) {
                grecaptcha.execute(t.b);
              }
            }
          });
        }));
      };
      /**
       * @return {undefined}
       */
      data.reset = function() {
        save(this);
        if (null !== this.b) {
          grecaptcha.reset(this.b);
        }
      };
      /**
       * @return {undefined}
       */
      data.clear = function() {
        save(this);
        /** @type {boolean} */
        this.o = true;
        splice().b--;
        /** @type {number} */
        var key = 0;
        for (; key < this.g.length; key++) {
          this.g[key].cancel("RecaptchaVerifier instance has been destroyed.");
        }
        if (!this.h) {
          key = removeEventListener(this.l);
          var value;
          for (; value = key.firstChild;) {
            key.removeChild(value);
          }
        }
      };
      var container = make("https://www.google.com/recaptcha/api.js?onload=%{onload}&render=explicit&hl=%{hl}");
      var clickedPlaylist = new link(3E4, 6E4);
      /** @type {null} */
      var Mc = null;
      expect(auth, run);
      /** @type {!Array<string>} */
      var shapes = "First Second Third Fourth Fifth Sixth Seventh Eighth Ninth".split(" ");
      _extend(init.prototype, {
        Ua : {
          name : "applyActionCode",
          i : [wrap("code")]
        },
        Ia : {
          name : "checkActionCode",
          i : [wrap("code")]
        },
        Wa : {
          name : "confirmPasswordReset",
          i : [wrap("code"), wrap("newPassword")]
        },
        Sb : {
          name : "createUserWithEmailAndPassword",
          i : [wrap("email"), wrap("password")]
        },
        sb : {
          name : "createUserAndRetrieveDataWithEmailAndPassword",
          i : [wrap("email"), wrap("password")]
        },
        Vb : {
          name : "fetchProvidersForEmail",
          i : [wrap("email")]
        },
        Wb : {
          name : "fetchSignInMethodsForEmail",
          i : [wrap("email")]
        },
        da : {
          name : "getRedirectResult",
          i : []
        },
        kc : {
          name : "isSignInWithEmailLink",
          i : [wrap("emailLink")]
        },
        rc : {
          name : "onAuthStateChanged",
          i : [registerAccount(deserialize(), createFilter(), "nextOrObserver"), createFilter("opt_error", true), createFilter("opt_completed", true)]
        },
        sc : {
          name : "onIdTokenChanged",
          i : [registerAccount(deserialize(), createFilter(), "nextOrObserver"), createFilter("opt_error", true), createFilter("opt_completed", true)]
        },
        gb : {
          name : "sendPasswordResetEmail",
          i : [wrap("email"), registerAccount(deserialize("opt_actionCodeSettings", true), _pretty_print_properties(null, true), "opt_actionCodeSettings", true)]
        },
        hb : {
          name : "sendSignInLinkToEmail",
          i : [wrap("email"), deserialize("actionCodeSettings")]
        },
        ib : {
          name : "setPersistence",
          i : [wrap("persistence")]
        },
        Oa : {
          name : "signInAndRetrieveDataWithCredential",
          i : [setData()]
        },
        jb : {
          name : "signInAnonymously",
          i : []
        },
        Jb : {
          name : "signInAnonymouslyAndRetrieveData",
          i : []
        },
        Bc : {
          name : "signInWithCredential",
          i : [setData()]
        },
        Cc : {
          name : "signInWithCustomToken",
          i : [wrap("token")]
        },
        Hb : {
          name : "signInAndRetrieveDataWithCustomToken",
          i : [wrap("token")]
        },
        Dc : {
          name : "signInWithEmailAndPassword",
          i : [wrap("email"), wrap("password")]
        },
        Ec : {
          name : "signInWithEmailLink",
          i : [wrap("email"), wrap("emailLink", true)]
        },
        Ib : {
          name : "signInAndRetrieveDataWithEmailAndPassword",
          i : [wrap("email"), wrap("password")]
        },
        Fc : {
          name : "signInWithPhoneNumber",
          i : [wrap("phoneNumber"), getShortcuts()]
        },
        Gc : {
          name : "signInWithPopup",
          i : [{
            name : "authProvider",
            K : "a valid Auth provider",
            optional : false,
            M : function(val) {
              return !!(val && val.providerId && val.hasOwnProperty && val.hasOwnProperty("isOAuthProvider"));
            }
          }]
        },
        Hc : {
          name : "signInWithRedirect",
          i : [{
            name : "authProvider",
            K : "a valid Auth provider",
            optional : false,
            M : function(val) {
              return !!(val && val.providerId && val.hasOwnProperty && val.hasOwnProperty("isOAuthProvider"));
            }
          }]
        },
        kb : {
          name : "signOut",
          i : []
        },
        toJSON : {
          name : "toJSON",
          i : [wrap(null, true)]
        },
        Oc : {
          name : "useDeviceLanguage",
          i : []
        },
        Pc : {
          name : "verifyPasswordResetCode",
          i : [wrap("code")]
        }
      });
      (function(BeautifulProperties, e) {
        var k;
        for (k in e) {
          var file = e[k].name;
          if (file !== k) {
            var header = e[k].Rb;
            Object.defineProperty(BeautifulProperties, file, {
              get : function() {
                return this[k];
              },
              set : function(name) {
                delegate(file, [header], [name], true);
                /** @type {string} */
                this[k] = name;
              },
              enumerable : true
            });
          }
        }
      })(init.prototype, {
        lc : {
          name : "languageCode",
          Rb : registerAccount(wrap(), _pretty_print_properties(), "languageCode")
        }
      });
      init.Persistence = tmp;
      /** @type {string} */
      init.Persistence.LOCAL = "local";
      /** @type {string} */
      init.Persistence.SESSION = "session";
      /** @type {string} */
      init.Persistence.NONE = "none";
      _extend(update.prototype, {
        delete : {
          name : "delete",
          i : []
        },
        ac : {
          name : "getIdTokenResult",
          i : [castTemplate()]
        },
        F : {
          name : "getIdToken",
          i : [castTemplate()]
        },
        getToken : {
          name : "getToken",
          i : [castTemplate()]
        },
        $a : {
          name : "linkAndRetrieveDataWithCredential",
          i : [setData()]
        },
        mc : {
          name : "linkWithCredential",
          i : [setData()]
        },
        nc : {
          name : "linkWithPhoneNumber",
          i : [wrap("phoneNumber"), getShortcuts()]
        },
        oc : {
          name : "linkWithPopup",
          i : [{
            name : "authProvider",
            K : "a valid Auth provider",
            optional : false,
            M : function(val) {
              return !!(val && val.providerId && val.hasOwnProperty && val.hasOwnProperty("isOAuthProvider"));
            }
          }]
        },
        pc : {
          name : "linkWithRedirect",
          i : [{
            name : "authProvider",
            K : "a valid Auth provider",
            optional : false,
            M : function(val) {
              return !!(val && val.providerId && val.hasOwnProperty && val.hasOwnProperty("isOAuthProvider"));
            }
          }]
        },
        bb : {
          name : "reauthenticateAndRetrieveDataWithCredential",
          i : [setData()]
        },
        vc : {
          name : "reauthenticateWithCredential",
          i : [setData()]
        },
        wc : {
          name : "reauthenticateWithPhoneNumber",
          i : [wrap("phoneNumber"), getShortcuts()]
        },
        xc : {
          name : "reauthenticateWithPopup",
          i : [{
            name : "authProvider",
            K : "a valid Auth provider",
            optional : false,
            M : function(val) {
              return !!(val && val.providerId && val.hasOwnProperty && val.hasOwnProperty("isOAuthProvider"));
            }
          }]
        },
        yc : {
          name : "reauthenticateWithRedirect",
          i : [{
            name : "authProvider",
            K : "a valid Auth provider",
            optional : false,
            M : function(val) {
              return !!(val && val.providerId && val.hasOwnProperty && val.hasOwnProperty("isOAuthProvider"));
            }
          }]
        },
        reload : {
          name : "reload",
          i : []
        },
        fb : {
          name : "sendEmailVerification",
          i : [registerAccount(deserialize("opt_actionCodeSettings", true), _pretty_print_properties(null, true), "opt_actionCodeSettings", true)]
        },
        toJSON : {
          name : "toJSON",
          i : [wrap(null, true)]
        },
        Mc : {
          name : "unlink",
          i : [wrap("provider")]
        },
        mb : {
          name : "updateEmail",
          i : [wrap("email")]
        },
        nb : {
          name : "updatePassword",
          i : [wrap("password")]
        },
        Nc : {
          name : "updatePhoneNumber",
          i : [setData("phone")]
        },
        ob : {
          name : "updateProfile",
          i : [deserialize("profile")]
        }
      });
      _extend(Promise.prototype, {
        ha : {
          name : "finally"
        },
        m : {
          name : "catch"
        },
        then : {
          name : "then"
        }
      });
      _extend(Server.prototype, {
        confirm : {
          name : "confirm",
          i : [wrap("verificationCode")]
        }
      });
      recurse(item, "credential", function(TemplateConfig, options) {
        return new Template(TemplateConfig, options);
      }, [wrap("email"), wrap("password")]);
      _extend(cache.prototype, {
        ta : {
          name : "addScope",
          i : [wrap("scope")]
        },
        Ca : {
          name : "setCustomParameters",
          i : [deserialize("customOAuthParameters")]
        }
      });
      recurse(cache, "credential", then, [registerAccount(wrap(), deserialize(), "token")]);
      recurse(item, "credentialWithLink", cleanup, [wrap("email"), wrap("emailLink")]);
      _extend(array.prototype, {
        ta : {
          name : "addScope",
          i : [wrap("scope")]
        },
        Ca : {
          name : "setCustomParameters",
          i : [deserialize("customOAuthParameters")]
        }
      });
      recurse(array, "credential", extract, [registerAccount(wrap(), deserialize(), "token")]);
      _extend(def.prototype, {
        ta : {
          name : "addScope",
          i : [wrap("scope")]
        },
        Ca : {
          name : "setCustomParameters",
          i : [deserialize("customOAuthParameters")]
        }
      });
      recurse(def, "credential", isValid, [registerAccount(wrap(), registerAccount(deserialize(), _pretty_print_properties()), "idToken"), registerAccount(wrap(), _pretty_print_properties(), "accessToken", true)]);
      _extend(template.prototype, {
        Ca : {
          name : "setCustomParameters",
          i : [deserialize("customOAuthParameters")]
        }
      });
      recurse(template, "credential", inArray, [registerAccount(wrap(), deserialize(), "token"), wrap("secret", true)]);
      _extend(self.prototype, {
        ta : {
          name : "addScope",
          i : [wrap("scope")]
        },
        credential : {
          name : "credential",
          i : [registerAccount(wrap(), _pretty_print_properties(), "idToken", true), registerAccount(wrap(), _pretty_print_properties(), "accessToken", true)]
        },
        Ca : {
          name : "setCustomParameters",
          i : [deserialize("customOAuthParameters")]
        }
      });
      recurse(d, "credential", navigate, [wrap("verificationId"), wrap("verificationCode")]);
      _extend(d.prototype, {
        Sa : {
          name : "verifyPhoneNumber",
          i : [wrap("phoneNumber"), getShortcuts()]
        }
      });
      _extend(Buffer.prototype, {
        toJSON : {
          name : "toJSON",
          i : [wrap(null, true)]
        }
      });
      _extend(Comment.prototype, {
        toJSON : {
          name : "toJSON",
          i : [wrap(null, true)]
        }
      });
      _extend(ui.prototype, {
        toJSON : {
          name : "toJSON",
          i : [wrap(null, true)]
        }
      });
      _extend(auth.prototype, {
        clear : {
          name : "clear",
          i : []
        },
        render : {
          name : "render",
          i : []
        },
        verify : {
          name : "verify",
          i : []
        }
      });
      (function() {
        if (void 0 === config || !config.INTERNAL || !config.INTERNAL.registerService) {
          throw Error("Cannot find the firebase namespace; be sure to include firebase-app.js before this library.");
        }
        var val = {
          Auth : init,
          Error : Buffer
        };
        recurse(val, "EmailAuthProvider", item, []);
        recurse(val, "FacebookAuthProvider", cache, []);
        recurse(val, "GithubAuthProvider", array, []);
        recurse(val, "GoogleAuthProvider", def, []);
        recurse(val, "TwitterAuthProvider", template, []);
        recurse(val, "OAuthProvider", self, [wrap("providerId")]);
        recurse(val, "PhoneAuthProvider", d, [{
          name : "auth",
          K : "an instance of Firebase Auth",
          optional : true,
          M : function(val) {
            return !!(val && val instanceof init);
          }
        }]);
        recurse(val, "RecaptchaVerifier", auth, [registerAccount(wrap(), {
          name : "",
          K : "an HTML element",
          optional : false,
          M : function(item) {
            return !!(item && item instanceof Element);
          }
        }, "recaptchaContainer"), deserialize("recaptchaParameters", true), {
          name : "app",
          K : "an instance of Firebase App",
          optional : true,
          M : function(value) {
            return !!(value && value instanceof config.app.App);
          }
        }]);
        config.INTERNAL.registerService("auth", function(that, extendApp) {
          return extendApp({
            INTERNAL : {
              getUid : bind((that = new init(that)).getUid, that),
              getToken : bind(that.$b, that),
              addAuthTokenListener : bind(that.Qb, that),
              removeAuthTokenListener : bind(that.zc, that)
            }
          }), that;
        }, val, function(type, object) {
          if ("create" === type) {
            try {
              object.auth();
            } catch (t) {
            }
          }
        });
        config.INTERNAL.extendNamespace({
          User : update
        });
      })();
    }).call(void 0 !== e ? e : "undefined" != typeof self ? self : "undefined" != typeof window ? window : {});
  } catch (t) {
    throw console.error(t), new Error("Cannot instantiate firebase-auth - be sure to load firebase-app.js first.");
  }
}(this.firebase = this.firebase || {}, firebase);
