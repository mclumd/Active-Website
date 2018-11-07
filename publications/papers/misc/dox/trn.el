;;(defun merge-line ()
;;  (let ((case-fold-search nil))
;;    (goto-char (point-min))
;;    (while (re-search-forward "\n........\.trn:\\([a-z]\\)" nil t)
;;      (replace-match " \\1" nil nil))))

(defun merge-line ()
  ;; extract turn indicators
  (goto-char (point-min))
  (while (re-search-forward "<u who=\\([FG]\\).*\n\\(<.*>.*\n\\)*" nil t)
    (replace-match "==\\1: " nil nil))

  ;; eliminate junk (markup tags)
  (goto-char (point-min))
  (while (re-search-forward "^<.*\n" nil t)
    (replace-match "" nil nil))

  ;; eliminate double spaces
  (goto-char (point-min))
  (while (re-search-forward "\n+" nil t)
    (replace-match "\n" nil nil))

  ;; merge lines 
  (goto-char (point-min))
  (while (re-search-forward "\n\\([^=]\\)" nil t)
    (replace-match " \\1" nil nil)))

(defun mark-context (string)
  (goto-char (point-min))
  (while (re-search-forward (concat "^"
				    "==G\\(.*\\b" string "\\b.*$\\)") nil t)
    (replace-match "=-G\\1" nil nil))

  (goto-char (point-min))
  (while (re-search-forward "\n" nil t)
    (replace-match "<br>\n" nil nil))

  (goto-char (point-min))
  (let* ((count 0)
	 (tname (number-to-string count)))
    (with-output-to-temp-buffer (concat "trn." string)

      (princ "<html><body>\n")
      
      (while (re-search-forward (concat "==\\(.*\\)\n"
					"==\\(.*\\)\n"
					"=-\\(.*\\)\n"
					"==\\(.*\\)\n"
					"==\\(.*\\)\n") nil t)
	(princ "<hr><p>\n")
	(mapc (lambda (index anchor)
		(princ (concat anchor (match-string index) "\n")))
	      '(1 2 3 4 5)
	      `("-2" "-1" ,(concat "<a href=\""
				   (buffer-name)
				   ".html#"
				   tname
				   "\">**</a>")
		"+1" "+2"))
	
	(replace-match (concat "<a name=\"" tname "\">"
			       "-2\\1\n"
			       "-1\\2\n" "<b>"
			       "-0\\3\n" "</b>"
			       "+1\\4\n"
			       "+2\\5\n") nil nil)
	(incf count)
	(setq tname (number-to-string count)))
      (princ "\n</body></html>")))

  (write-file (concat (buffer-name) ".html")))
