(defun index-char (i)
  (cond
    ((< i 26) (code-char (+ #x41 i)))
    ((< i 52) (code-char (+ #x61 -26 i)))
    ((< i 62) (code-char (+ #x30 -52 i)))
    ((= i 62) #\+)
    ((= i 63) #\/)
    (T (error "Invalid index: ~a. Must be in the range [0,63]" i))))

(defun index-char-table ()
  (loop for i from 0 below (expt 2 6)
        do (format T "'~a', " (index-char i)))
  (terpri))

(defun char-index-table ()
  (let ((small-table (make-hash-table)))
    (loop for i from 0 below (expt 2 6) 
          do (setf (gethash (char-code (index-char i)) small-table) i))
    (loop for i from 0 below (expt 2 8)
          do (format T "~a, "
                     (if (char= (code-char i) #\=)
                       0
                       (gethash i small-table (expt 2 6)))))))
